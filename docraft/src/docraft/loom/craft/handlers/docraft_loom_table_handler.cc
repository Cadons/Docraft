/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "docraft/loom/craft/handlers/docraft_loom_table_handler.h"

#include <any>

#include <nlohmann/json.hpp>

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/templating/docraft_template_engine.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::craft {
    namespace tokens = docraft::craft;
    namespace parser = docraft::craft::parser;

    namespace {
        std::shared_ptr<nodes::DocraftLoomTableCell> build_matrix_cell(const std::string& text)
        {
            auto cell = std::make_shared<nodes::DocraftLoomTableCell>();
            cell->set_content(std::make_shared<nodes::DocraftLoomText>(text));
            return cell;
        }

        // RAII guard that overrides the context's current Foreach item for its lifetime
        // and restores the previous value on scope exit (including via an exception) --
        // used by build_templated_model_rows() to bind each JSON-model object to its
        // cloned row template.
        class ForeachItemScope
        {
        public:
            ForeachItemScope(DocraftLoomTableHandlerContext& context, const nlohmann::json* value)
                : context_(context), previous_(context.current_foreach_item())
            {
                context_.set_current_foreach_item(value);
            }
            ~ForeachItemScope() { context_.set_current_foreach_item(previous_); }
            ForeachItemScope(const ForeachItemScope&) = delete;
            ForeachItemScope& operator=(const ForeachItemScope&) = delete;

        private:
            DocraftLoomTableHandlerContext& context_;
            const nlohmann::json* previous_;
        };
    } // namespace

    std::shared_ptr<nodes::DocraftLoomTableCell> DocraftLoomTableHandler::build_title_cell(
        const parser::ParsedTableTitleData& title, DocraftLoomTableHandlerContext& context)
    {
        auto text = std::make_shared<nodes::DocraftLoomText>(
            context.template_engine().render_template_string(title.text));
        text->set_alignment(to_loom_alignment(title.alignment));
        apply_style(*text, title.style);
        if (title.color)
        {
            text->set_color(context.resolve_color(*title.color));
        }
        if (title.font_size)
        {
            text->set_font_size(*title.font_size);
        }
        auto cell = std::make_shared<nodes::DocraftLoomTableCell>();
        cell->set_content(text);
        cell->set_is_title(true);
        if (title.background)
        {
            cell->set_background(context.resolve_color(*title.background));
        }
        return cell;
    }

    std::shared_ptr<nodes::DocraftLoomTableCell> DocraftLoomTableHandler::build_content_cell(
        const parser::ParsedTableCellData& cell_data, DocraftLoomTableHandlerContext& context)
    {
        auto cell = std::make_shared<nodes::DocraftLoomTableCell>();

        // Cell content is built directly from ParsedTableCellData rather than via the
        // generic tag-handler dispatch (Table content isn't recursed into generically --
        // see DocraftLoomTableHandler::build()), so the usual "visible" check never runs
        // for it; replicate that check here, or an explicitly-invisible Text/Image inside
        // a <Cell> would still render, unlike everywhere else in the document.
        const bool visible = !cell_data.content_common.visible.has_value() || *cell_data.content_common.visible;
        if (visible)
        {
            std::shared_ptr<nodes::DocraftLoomNode> content;
            if (cell_data.content_tag_name == std::string{tokens::elements::kText})
            {
                auto text_node = std::make_shared<nodes::DocraftLoomText>();
                context.fill_text_node(*text_node, std::any_cast<const parser::ParsedTextData&>(cell_data.content));
                apply_common_attributes(*text_node, cell_data.content_common);
                content = text_node;
            }
            else
            {
                auto image_node = std::make_shared<nodes::DocraftLoomImage>();
                context.fill_image_node(*image_node, std::any_cast<const parser::ParsedImageData&>(cell_data.content));
                apply_common_attributes(*image_node, cell_data.content_common);
                content = image_node;
            }
            cell->set_content(content);
        }

        if (cell_data.background)
        {
            cell->set_background(context.resolve_color(*cell_data.background));
        }
        if (cell_data.width)
        {
            cell->set_explicit_width(*cell_data.width);
        }
        return cell;
    }

    nlohmann::json DocraftLoomTableHandler::resolve_table_model_json(const std::string& raw,
                                                                     DocraftLoomTableHandlerContext& context)
    {
        const std::string normalized = normalize_single_quoted_json(context.render_template_text(raw));

        nlohmann::json parsed;
        try
        {
            parsed = nlohmann::json::parse(normalized);
        }
        catch (const nlohmann::json::exception& e)
        {
            throw docraft::exception::DataFormatException(
                std::string{"<Table> 'model' is not valid JSON: "} + e.what());
        }
        if (!parsed.is_array() || parsed.empty())
        {
            throw docraft::exception::DataFormatException("<Table> 'model' must resolve to a non-empty JSON array");
        }
        return parsed;
    }

    std::vector<std::vector<std::string>> DocraftLoomTableHandler::to_string_matrix(const nlohmann::json& parsed)
    {
        std::vector<std::vector<std::string>> matrix;
        matrix.reserve(parsed.size());
        for (const auto& row : parsed)
        {
            if (!row.is_array() || row.empty())
            {
                throw docraft::exception::DataFormatException(
                    "<Table> 'model' rows must be non-empty JSON arrays");
            }
            if (!matrix.empty() && row.size() != matrix.front().size())
            {
                throw docraft::exception::DataFormatException(
                    "<Table> 'model' rows must all have the same length");
            }
            std::vector<std::string> parsed_row;
            parsed_row.reserve(row.size());
            for (const auto& cell : row)
            {
                if (!cell.is_string())
                {
                    throw docraft::exception::DataFormatException("<Table> 'model' cells must be strings");
                }
                parsed_row.push_back(cell.get<std::string>());
            }
            matrix.push_back(std::move(parsed_row));
        }
        return matrix;
    }

    std::vector<std::string> DocraftLoomTableHandler::resolve_table_header(const std::string& raw,
                                                                           DocraftLoomTableHandlerContext& context)
    {
        const std::string normalized = normalize_single_quoted_json(context.render_template_text(raw));

        nlohmann::json parsed;
        try
        {
            parsed = nlohmann::json::parse(normalized);
        }
        catch (const nlohmann::json::exception& e)
        {
            throw docraft::exception::DataFormatException(
                std::string{"<Table> 'header' is not valid JSON: "} + e.what());
        }
        if (!parsed.is_array() || parsed.empty())
        {
            throw docraft::exception::DataFormatException(
                "<Table> 'header' must resolve to a non-empty JSON array");
        }

        std::vector<std::string> header;
        header.reserve(parsed.size());
        for (const auto& item : parsed)
        {
            if (!item.is_string())
            {
                throw docraft::exception::DataFormatException("<Table> 'header' items must be strings");
            }
            header.push_back(item.get<std::string>());
        }
        return header;
    }

    void DocraftLoomTableHandler::add_table_model_header_row(nodes::DocraftLoomTable& table,
                                                             const parser::ParsedTableData& data,
                                                             std::size_t column_count,
                                                             DocraftLoomTableHandlerContext& context)
    {
        if (data.header_data_template)
        {
            const std::vector<std::string> header = resolve_table_header(*data.header_data_template, context);
            if (header.size() != column_count)
            {
                throw docraft::exception::DataFormatException(
                    "<Table> 'header' size must match 'model' column count");
            }
            std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> header_row;
            header_row.reserve(header.size());
            for (const auto& title : header)
            {
                parser::ParsedTableTitleData title_data;
                title_data.text = title;
                header_row.push_back(build_title_cell(title_data, context));
            }
            table.add_row(header_row);
            return;
        }

        if (!data.header_titles.empty())
        {
            // An explicit <THead> can be used instead of the `header` attribute alongside a
            // JSON/template `model` -- the parser doesn't reject that combination (only
            // `header` + <THead> together is rejected), so it must be honored here too, not
            // just the header_data_template path above.
            if (data.header_titles.size() != column_count)
            {
                throw docraft::exception::DataFormatException(
                    "<Table> THead column count must match 'model' column count");
            }
            std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> header_row;
            header_row.reserve(data.header_titles.size());
            for (const auto& title : data.header_titles)
            {
                header_row.push_back(build_title_cell(title, context));
            }
            table.add_row(header_row);
        }
    }

    void DocraftLoomTableHandler::build_templated_model_rows(nodes::DocraftLoomTable& table,
                                                             const nlohmann::json& model_json,
                                                             const parser::ParsedTableData& data,
                                                             DocraftLoomTableHandlerContext& context)
    {
        if (data.rows.empty())
        {
            throw docraft::exception::InvalidInputException(
                "<Table> 'model' resolving to a JSON array of objects requires an explicit <TBody> row template");
        }

        add_table_model_header_row(table, data, data.rows.front().cells.size(), context);

        for (const auto& item : model_json)
        {
            if (!item.is_object())
            {
                throw docraft::exception::DataFormatException(
                    "<Table> 'model' must be a JSON array of only objects or only arrays, not mixed");
            }
            // Scoped per iteration (rather than a single save/restore around the whole
            // loop) so the context's current Foreach item is restored to whatever it was
            // before this call -- the outer <Foreach> item, if any -- even if building a
            // row throws.
            ForeachItemScope item_scope(context, &item);
            for (const auto& row_data : data.rows)
            {
                std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> row;
                row.reserve(row_data.cells.size());
                for (const auto& cell_data : row_data.cells)
                {
                    row.push_back(build_content_cell(cell_data, context));
                }
                table.add_row(row);
            }
        }
    }

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomTableHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const parser::ParsedTableData&>(element.data);
        auto table = std::make_shared<nodes::DocraftLoomTable>();

        // Table structure is parsed specially (no generic child recursion -- see
        // DocraftCraftLanguageParser::parse_node's Table early-return), so a <Foreach>
        // can't appear as a literal child of a <Table>. That doesn't mean the context's
        // current Foreach item should be hidden from this Table's own attributes/content,
        // though: if the Table itself is nested inside an outer <Foreach>, its
        // `model`/`header` attributes and its cell content must still resolve
        // `${data("field")}` against that outer item -- it is left untouched here and
        // simply flows through resolve_table_model_json()/resolve_table_header()/
        // build_content_cell() (via render_template_text()). build_templated_model_rows()
        // below additionally overrides it per JSON-model object, scoped to that object's
        // own rows.
        if (data.baseline_offset)
        {
            table->set_baseline_offset(*data.baseline_offset);
        }
        if (data.default_cell_background)
        {
            table->set_default_cell_background(context.resolve_color(*data.default_cell_background));
        }

        const bool is_vertical = data.orientation == parser::ParsedTableOrientation::kVertical;

        if (data.model_data_template)
        {
            const nlohmann::json model_json = resolve_table_model_json(*data.model_data_template, context);

            if (model_json.front().is_object())
            {
                // `model` is a JSON array of objects -- the explicit <TBody> (already parsed
                // into data.rows) is used as a per-object row template rather than literal
                // rows; see build_templated_model_rows().
                build_templated_model_rows(*table, model_json, data, context);
                apply_common_attributes(*table, element.common);
                return table;
            }

            // `model` is a JSON array of arrays (a string matrix): an explicit <TBody> has
            // no role here (there's no per-object data to bind it against), so require the
            // author to pick one or the other rather than silently dropping their <TBody>.
            if (!data.rows.empty())
            {
                throw docraft::exception::InvalidInputException(
                    "<Table> 'model' resolving to a JSON array of arrays cannot be combined with an explicit <TBody>");
            }

            const std::vector<std::vector<std::string>> matrix = to_string_matrix(model_json);
            add_table_model_header_row(*table, data, matrix.front().size(), context);

            for (const auto& matrix_row : matrix)
            {
                std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> row;
                row.reserve(matrix_row.size());
                for (const auto& cell_text : matrix_row)
                {
                    row.push_back(build_matrix_cell(cell_text));
                }
                table->add_row(row);
            }

            apply_common_attributes(*table, element.common);
            return table;
        }

        if (!data.header_titles.empty())
        {
            std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> header_row;
            if (is_vertical)
            {
                // The label column has no header of its own -- pad with a blank title
                // cell so the header row aligns under the row-title column.
                auto corner = std::make_shared<nodes::DocraftLoomTableCell>();
                corner->set_content(std::make_shared<nodes::DocraftLoomText>(""));
                corner->set_is_title(true);
                header_row.push_back(corner);
            }
            for (const auto& title : data.header_titles)
            {
                header_row.push_back(build_title_cell(title, context));
            }
            table->add_row(header_row);
        }

        for (const auto& row_data : data.rows)
        {
            std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> row;
            if (is_vertical && row_data.row_title)
            {
                row.push_back(build_title_cell(*row_data.row_title, context));
            }
            for (const auto& cell_data : row_data.cells)
            {
                row.push_back(build_content_cell(cell_data, context));
            }
            table->add_row(row);
        }

        apply_common_attributes(*table, element.common);
        return table;
    }
} // namespace docraft::loom::craft

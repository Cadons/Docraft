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

#include "docraft/loom/craft/docraft_loom_tree_builder.h"

#include <algorithm>
#include <any>
#include <cctype>
#include <string_view>

#include <nlohmann/json.hpp>

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_circle_parser.h"
#include "docraft/craft/parser/docraft_foreach_parser.h"
#include "docraft/craft/parser/docraft_line_parser.h"
#include "docraft/craft/parser/docraft_paragraph_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/craft/parser/docraft_polygon_parser.h"
#include "docraft/craft/parser/docraft_section_parsers.h"
#include "docraft/craft/parser/docraft_triangle_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::craft {
    DocraftLoomTreeBuilder::DocraftLoomTreeBuilder(
        std::shared_ptr<docraft::templating::DocraftTemplateEngine> template_engine)
        : template_engine_(template_engine
                               ? std::move(template_engine)
                               : std::make_shared<docraft::templating::DocraftTemplateEngine>())
    {
    }

    namespace tokens = docraft::craft;
    namespace parser = docraft::craft::parser;

    namespace {
        // Normalizes a JSON string that may use single quotes for strings instead of
        // double quotes (the `.craft` convention for embedding JSON in an XML attribute,
        // e.g. `model='[{"a":1}]'` vs `model="[{'a':1}]"`), while preserving apostrophes
        // that are part of a word (e.g. "don't") rather than string delimiters. Applied
        // after ${variable}/${data(...)} resolution, since a resolved variable's value may
        // itself use this convention.
        bool is_word_char(char ch)
        {
            const auto uch = static_cast<unsigned char>(ch);
            return std::isalnum(uch) != 0 || ch == '_';
        }

        std::string normalize_single_quoted_json(std::string_view input)
        {
            std::string output;
            output.reserve(input.size());
            bool in_single = false;
            bool in_double = false;
            for (std::size_t i = 0; i < input.size(); ++i)
            {
                const char ch = input[i];
                if (ch == '\\')
                {
                    if (in_single && (i + 1) < input.size())
                    {
                        const char next = input[i + 1];
                        if (next == '\\' || next == '\'')
                        {
                            output.push_back(next);
                            ++i;
                            continue;
                        }
                    }
                    output.push_back(ch);
                    continue;
                }
                if (ch == '\'' && !in_double)
                {
                    if (in_single)
                    {
                        const char prev = (i > 0) ? input[i - 1] : '\0';
                        const char next = ((i + 1) < input.size()) ? input[i + 1] : '\0';
                        if (is_word_char(prev) && is_word_char(next))
                        {
                            output.push_back('\'');
                            continue;
                        }
                    }
                    in_single = !in_single;
                    output.push_back('"');
                    continue;
                }
                if (ch == '"' && !in_single)
                {
                    in_double = !in_double;
                    output.push_back(ch);
                    continue;
                }
                output.push_back(ch);
            }
            return output;
        }

        void apply_style(nodes::DocraftLoomText& node, parser::ParsedTextStyle style)
        {
            switch (style)
            {
            case parser::ParsedTextStyle::kBold:
                node.set_bold(true);
                node.set_italic(false);
                break;
            case parser::ParsedTextStyle::kItalic:
                node.set_bold(false);
                node.set_italic(true);
                break;
            case parser::ParsedTextStyle::kBoldItalic:
                node.set_bold(true);
                node.set_italic(true);
                break;
            case parser::ParsedTextStyle::kNormal:
            default:
                node.set_bold(false);
                node.set_italic(false);
                break;
            }
        }

        nodes::TextAlignment to_loom_alignment(parser::ParsedTextAlignment alignment)
        {
            switch (alignment)
            {
            case parser::ParsedTextAlignment::kLeft:
                return nodes::TextAlignment::kLeft;
            case parser::ParsedTextAlignment::kRight:
                return nodes::TextAlignment::kRight;
            case parser::ParsedTextAlignment::kJustified:
                return nodes::TextAlignment::kJustified;
            case parser::ParsedTextAlignment::kCenter:
            default:
                return nodes::TextAlignment::kCenter;
            }
        }

        void fill_text_node(nodes::DocraftLoomText& node, const parser::ParsedTextData& data,
                            const docraft::templating::DocraftTemplateEngine& engine,
                            const nlohmann::json* foreach_item)
        {
            node.set_text(engine.render_template_string(data.text, foreach_item));
            if (data.font_size)
            {
                node.set_font_size(*data.font_size);
            }
            if (data.font_name)
            {
                node.set_font_family(*data.font_name);
            }
            if (data.color)
            {
                node.set_color(*data.color);
            }
            if (data.style)
            {
                apply_style(node, *data.style);
            }
            if (data.alignment)
            {
                node.set_alignment(to_loom_alignment(*data.alignment));
            }
            if (data.underline)
            {
                node.set_underline(*data.underline);
            }
            if (data.strikeout)
            {
                node.set_strikeout(*data.strikeout);
            }
        }

        void fill_page_number_node(nodes::DocraftLoomPageNumber& node, const parser::ParsedPageNumberData& data)
        {
            if (data.font_size)
            {
                node.set_font_size(*data.font_size);
            }
            if (data.font_name)
            {
                node.set_font_family(*data.font_name);
            }
            if (data.color)
            {
                node.set_color(*data.color);
            }
            if (data.style)
            {
                apply_style(node, *data.style);
            }
            if (data.alignment)
            {
                node.set_alignment(to_loom_alignment(*data.alignment));
            }
            if (data.underline)
            {
                node.set_underline(*data.underline);
            }
            if (data.strikeout)
            {
                node.set_strikeout(*data.strikeout);
            }
        }

        void fill_image_node(nodes::DocraftLoomImage& node, const parser::ParsedImageData& data,
                             const docraft::templating::DocraftTemplateEngine& engine,
                             const nlohmann::json* foreach_item)
        {
            if (data.raw_data && data.raw_pixel_width && data.raw_pixel_height)
            {
                node.set_raw_data(*data.raw_data, *data.raw_pixel_width, *data.raw_pixel_height);
            }
            else if (data.path)
            {
                node.set_path(engine.render_template_string(*data.path, foreach_item));
            }
        }

        std::shared_ptr<nodes::DocraftLoomTableCell> build_title_cell(
            const parser::ParsedTableTitleData& title, const docraft::templating::DocraftTemplateEngine& engine)
        {
            auto text = std::make_shared<nodes::DocraftLoomText>(engine.render_template_string(title.text));
            text->set_alignment(to_loom_alignment(title.alignment));
            apply_style(*text, title.style);
            if (title.color)
            {
                text->set_color(*title.color);
            }
            auto cell = std::make_shared<nodes::DocraftLoomTableCell>();
            cell->set_content(text);
            cell->set_is_title(true);
            if (title.background)
            {
                cell->set_background(*title.background);
            }
            return cell;
        }

        std::shared_ptr<nodes::DocraftLoomTableCell> build_content_cell(
            const parser::ParsedTableCellData& cell_data, const docraft::templating::DocraftTemplateEngine& engine)
        {
            std::shared_ptr<nodes::DocraftLoomNode> content;
            if (cell_data.content_tag_name == std::string{tokens::elements::kText})
            {
                auto text_node = std::make_shared<nodes::DocraftLoomText>();
                // Table structure is parsed specially (no generic child recursion -- see
                // DocraftCraftLanguageParser::parse_node's Table early-return), so a
                // <Foreach> can't appear inside a <Table>; no foreach item to bind here,
                // but global ${variable} substitution still applies.
                fill_text_node(*text_node, std::any_cast<const parser::ParsedTextData&>(cell_data.content), engine,
                               nullptr);
                content = text_node;
            }
            else
            {
                auto image_node = std::make_shared<nodes::DocraftLoomImage>();
                fill_image_node(*image_node, std::any_cast<const parser::ParsedImageData&>(cell_data.content), engine,
                                nullptr);
                content = image_node;
            }

            auto cell = std::make_shared<nodes::DocraftLoomTableCell>();
            cell->set_content(content);
            if (cell_data.background)
            {
                cell->set_background(*cell_data.background);
            }
            if (cell_data.width)
            {
                cell->set_explicit_width(*cell_data.width);
            }
            return cell;
        }

        std::shared_ptr<nodes::DocraftLoomTableCell> build_matrix_cell(const std::string& text)
        {
            auto cell = std::make_shared<nodes::DocraftLoomTableCell>();
            cell->set_content(std::make_shared<nodes::DocraftLoomText>(text));
            return cell;
        }

        // Resolves a `<Table model="...">` attribute (already known not to be the
        // "horizontal"/"vertical" keyword) into a rectangular, string-only matrix:
        // `${...}`/`${data(...)}` substitution (no foreach item -- Table isn't nested in a
        // Foreach), then the same single-quote-JSON normalization Foreach's model uses,
        // then JSON-parse.
        std::vector<std::vector<std::string>> resolve_table_model_matrix(
            const std::string& raw, const docraft::templating::DocraftTemplateEngine& engine)
        {
            const std::string normalized = normalize_single_quoted_json(engine.render_template_string(raw));

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

        // Resolves a `<Table header="...">` attribute into a non-empty, string-only array,
        // using the same `${...}` substitution + single-quote-JSON normalization as the
        // model matrix above.
        std::vector<std::string> resolve_table_header(
            const std::string& raw, const docraft::templating::DocraftTemplateEngine& engine)
        {
            const std::string normalized = normalize_single_quoted_json(engine.render_template_string(raw));

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
    } // namespace

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomTreeBuilder::build(
        const std::shared_ptr<docraft::craft::DocraftParsedElement>& element)
    {
        if (!element)
        {
            return nullptr;
        }
        // "visible" is resolved here, not by the (engine-agnostic) craft parser: an
        // explicitly-invisible element's subtree simply isn't constructed.
        if (element->common.visible.has_value() && !*element->common.visible)
        {
            return nullptr;
        }

        const std::string& tag = element->tag_name;
        if (tag == std::string{tokens::elements::kRectangle})
        {
            return build_rectangle(*element);
        }
        if (tag == std::string{tokens::elements::kCircle})
        {
            return build_circle(*element);
        }
        if (tag == std::string{tokens::elements::kTriangle})
        {
            return build_triangle(*element);
        }
        if (tag == std::string{tokens::elements::kPolygon})
        {
            return build_polygon(*element);
        }
        if (tag == std::string{tokens::elements::kLine})
        {
            return build_line(*element);
        }
        if (tag == std::string{tokens::elements::kText})
        {
            return build_text(*element);
        }
        if (tag == std::string{tokens::elements::kTitle})
        {
            return build_title(*element);
        }
        if (tag == std::string{tokens::elements::kSubtitle})
        {
            return build_subtitle(*element);
        }
        if (tag == std::string{tokens::elements::kPageNumber})
        {
            return build_page_number(*element);
        }
        if (tag == std::string{tokens::elements::kImage})
        {
            return build_image(*element);
        }
        if (tag == std::string{tokens::elements::kBlankLine})
        {
            return build_blank_line(*element);
        }
        if (tag == std::string{tokens::elements::kNewPage})
        {
            return build_new_page(*element);
        }
        if (tag == std::string{tokens::elements::kList} || tag == std::string{tokens::elements::kUList})
        {
            return build_list(*element);
        }
        if (tag == std::string{tokens::elements::kTable})
        {
            return build_table(*element);
        }
        if (tag == std::string{tokens::elements::kLayout})
        {
            return build_layout(*element);
        }
        if (tag == std::string{tokens::elements::kParagraph})
        {
            return build_paragraph(*element);
        }
        if (tag == std::string{tokens::section::kHeader} || tag == std::string{tokens::section::kBody} ||
            tag == std::string{tokens::section::kFooter})
        {
            return build_section(*element);
        }

        throw docraft::exception::DataFormatException("DocraftLoomTreeBuilder: unrecognized tag '" + tag + "'");
    }

    void DocraftLoomTreeBuilder::add_children(const std::shared_ptr<nodes::DocraftLoomNode>& parent,
                                              const std::vector<std::shared_ptr<ParsedElement>>& children)
    {
        for (const auto& child : children)
        {
            if (child->tag_name == std::string{tokens::elements::templating::kForeach})
            {
                expand_foreach(parent, *child);
                continue;
            }
            if (auto built = build(child))
            {
                parent->add_child(built);
            }
        }
    }

    void DocraftLoomTreeBuilder::expand_foreach(const std::shared_ptr<nodes::DocraftLoomNode>& parent,
                                                const ParsedElement& foreach_element)
    {
        const auto& data = std::any_cast<const parser::ParsedForeachData&>(foreach_element.data);

        if (data.model)
        {
            // `model` may itself be (or contain) a `${variable}`/`${data("field")}`
            // expression -- e.g. model="${employees}", or, nested inside an outer
            // Foreach, model="${data("subitems")}" -- so it must be resolved against the
            // *outer* current_foreach_item_ (still in scope at this point) before the
            // single-quote normalization and JSON parsing below.
            const std::string resolved_model =
                template_engine_->render_template_string(*data.model, current_foreach_item_);
            const std::string normalized_model = normalize_single_quoted_json(resolved_model);

            nlohmann::json items;
            try
            {
                items = nlohmann::json::parse(normalized_model);
            }
            catch (const nlohmann::json::exception& e)
            {
                throw docraft::exception::DataFormatException(
                    std::string{"<Foreach> 'model' is not valid JSON: "} + e.what());
            }
            if (!items.is_array())
            {
                throw docraft::exception::DataFormatException("<Foreach> 'model' must resolve to a JSON array");
            }

            const nlohmann::json* previous_item = current_foreach_item_;
            for (const auto& item : items)
            {
                current_foreach_item_ = &item;
                add_children(parent, foreach_element.children);
            }
            current_foreach_item_ = previous_item;
            return;
        }

        const int count = data.count.value_or(0);
        for (int i = 0; i < count; ++i)
        {
            add_children(parent, foreach_element.children);
        }
    }

    template <typename NodeT>
    void DocraftLoomTreeBuilder::apply_common_attributes(NodeT& node,
                                                         const docraft::craft::DocraftCommonAttributes& common)
    {
        // name/z_index/position mode/explicit position/padding/margin live on
        // DocraftLoomNode itself, so every node type has them -- no gating needed.
        if (common.name)
        {
            node.set_name(*common.name);
        }
        if (common.z_index)
        {
            node.set_z_index(*common.z_index);
        }
        if (common.padding)
        {
            node.set_padding(*common.padding);
        }
        if (common.margin)
        {
            node.set_margin(*common.margin);
        }
        if (common.margin_top || common.margin_right || common.margin_bottom || common.margin_left)
        {
            const auto& current = node.margin();
            node.set_margin(common.margin_top.value_or(current.top), common.margin_right.value_or(current.right),
                             common.margin_bottom.value_or(current.bottom), common.margin_left.value_or(current.left));
        }

        // width/height/weight vary per node type -- skip silently when NodeT has no
        // matching setter, rather than failing to compile or throwing.
        if constexpr (requires(NodeT& n, float v) { n.set_width(v); })
        {
            if (common.width)
            {
                node.set_width(*common.width);
            }
        }
        if constexpr (requires(NodeT& n, float v) { n.set_height(v); })
        {
            if (common.height)
            {
                node.set_height(*common.height);
            }
        }
        if constexpr (requires(NodeT& n, float v) { n.set_weight(v); })
        {
            if (common.weight)
            {
                node.set_weight(*common.weight);
            }
        }

        if (common.position_mode)
        {
            node.set_position_mode(*common.position_mode == docraft::craft::PositionMode::kAbsolute
                                       ? nodes::DocraftPositionType::kAbsolute
                                       : nodes::DocraftPositionType::kBlock);
        }
        if (common.x || common.y)
        {
            nodes::Position pos = node.explicit_position();
            if (common.x)
            {
                pos.x = *common.x;
            }
            if (common.y)
            {
                pos.y = *common.y;
            }
            node.set_explicit_position(pos);
        }
    }

    std::shared_ptr<nodes::DocraftLoomRectangle> DocraftLoomTreeBuilder::build_rectangle(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedRectangleData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomRectangle>();
        if (data.background_color)
        {
            node->edit_style().background_color = *data.background_color;
        }
        if (data.border_color)
        {
            node->edit_style().border_color = *data.border_color;
        }
        if (data.border_width)
        {
            node->edit_style().border_width = *data.border_width;
        }
        apply_common_attributes(*node, element.common);
        add_children(node, element.children);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomCircle> DocraftLoomTreeBuilder::build_circle(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedCircleData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomCircle>();
        if (data.background_color)
        {
            node->edit_style().background_color = *data.background_color;
        }
        if (data.border_color)
        {
            node->edit_style().border_color = *data.border_color;
        }
        if (data.border_width)
        {
            node->edit_style().border_width = *data.border_width;
        }
        if (data.radius)
        {
            node->set_radius(*data.radius);
        }
        // Note: common.width/common.height are deliberately not consulted here -- Circle
        // has its own `radius` attribute instead, and DocraftLoomCircle has no
        // set_width()/set_height() for apply_common_attributes to gate on either.
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomTriangle> DocraftLoomTreeBuilder::build_triangle(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedTriangleData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomTriangle>();
        if (data.background_color)
        {
            node->edit_style().background_color = *data.background_color;
        }
        if (data.border_color)
        {
            node->edit_style().border_color = *data.border_color;
        }
        if (data.border_width)
        {
            node->edit_style().border_width = *data.border_width;
        }
        if (!data.points.empty())
        {
            node->set_points(data.points);
        }
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomPolygon> DocraftLoomTreeBuilder::build_polygon(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedPolygonData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomPolygon>();
        if (data.background_color)
        {
            node->edit_style().background_color = *data.background_color;
        }
        if (data.border_color)
        {
            node->edit_style().border_color = *data.border_color;
        }
        if (data.border_width)
        {
            node->edit_style().border_width = *data.border_width;
        }
        if (!data.points.empty())
        {
            node->set_points(data.points);
        }
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomLine> DocraftLoomTreeBuilder::build_line(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedLineData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomLine>();
        nodes::Position start = node->start();
        nodes::Position end = node->end();
        if (data.x1)
        {
            start.x = *data.x1;
        }
        if (data.y1)
        {
            start.y = *data.y1;
        }
        if (data.x2)
        {
            end.x = *data.x2;
        }
        if (data.y2)
        {
            end.y = *data.y2;
        }
        node->set_start(start);
        node->set_end(end);
        if (data.border_color)
        {
            node->set_border_color(*data.border_color);
        }
        if (data.border_width)
        {
            node->set_border_width(*data.border_width);
        }
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomText> DocraftLoomTreeBuilder::build_text(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedTextData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomText>();
        fill_text_node(*node, data, *template_engine_, current_foreach_item_);
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomTitle> DocraftLoomTreeBuilder::build_title(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedTextData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomTitle>();
        // fill_text_node only overwrites fields data actually set (see its own
        // if (data.xxx) checks) -- an explicit font-size attribute still wins,
        // otherwise DocraftLoomTitle's own constructor default stands.
        fill_text_node(*node, data, *template_engine_, current_foreach_item_);
        // Keep margin coherent with whatever font_size ended up being used (constructor
        // default or an explicit attribute) -- 1em, matching the constructor's own
        // convention -- so overriding font-size doesn't leave a disproportionate,
        // fixed-constant gap. An explicit margin attribute below still overrides this.
        node->set_margin(node->font_size());
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomSubtitle> DocraftLoomTreeBuilder::build_subtitle(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedTextData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomSubtitle>();
        fill_text_node(*node, data, *template_engine_, current_foreach_item_);
        // See build_title's identical comment above.
        node->set_margin(node->font_size());
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomPageNumber> DocraftLoomTreeBuilder::build_page_number(
        const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedPageNumberData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomPageNumber>();
        fill_page_number_node(*node, data);
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomImage> DocraftLoomTreeBuilder::build_image(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedImageData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomImage>();
        fill_image_node(*node, data, *template_engine_, current_foreach_item_);
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomBlankLine> DocraftLoomTreeBuilder::build_blank_line(const ParsedElement& element)
    {
        auto node = std::make_shared<nodes::DocraftLoomBlankLine>();
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomNewPage> DocraftLoomTreeBuilder::build_new_page(const ParsedElement& element)
    {
        auto node = std::make_shared<nodes::DocraftLoomNewPage>();
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomList> DocraftLoomTreeBuilder::build_list(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedListData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomList>();
        node->set_kind(data.kind == parser::ParsedListKind::kOrdered
                           ? nodes::ListKind::kOrdered
                           : nodes::ListKind::kUnordered);
        if (data.ordered_style)
        {
            node->set_ordered_style(*data.ordered_style == parser::ParsedOrderedListStyle::kNumber
                                        ? nodes::OrderedListStyle::kNumber
                                        : nodes::OrderedListStyle::kRoman);
        }
        if (data.unordered_dot)
        {
            nodes::UnorderedListDot dot = nodes::UnorderedListDot::kCircle;
            switch (*data.unordered_dot)
            {
            case parser::ParsedUnorderedListDot::kDash:
                dot = nodes::UnorderedListDot::kDash;
                break;
            case parser::ParsedUnorderedListDot::kStar:
                dot = nodes::UnorderedListDot::kStar;
                break;
            case parser::ParsedUnorderedListDot::kBox:
                dot = nodes::UnorderedListDot::kBox;
                break;
            case parser::ParsedUnorderedListDot::kCircle:
            default:
                dot = nodes::UnorderedListDot::kCircle;
                break;
            }
            node->set_unordered_dot(dot);
        }
        apply_common_attributes(*node, element.common);
        add_children(node, element.children);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomTable> DocraftLoomTreeBuilder::build_table(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedTableData&>(element.data);
        auto table = std::make_shared<nodes::DocraftLoomTable>();
        if (data.baseline_offset)
        {
            table->set_baseline_offset(*data.baseline_offset);
        }
        if (data.default_cell_background)
        {
            table->set_default_cell_background(*data.default_cell_background);
        }

        const bool is_vertical = data.orientation == parser::ParsedTableOrientation::kVertical;

        if (data.model_data_template)
        {
            const std::vector<std::vector<std::string>> matrix =
                resolve_table_model_matrix(*data.model_data_template, *template_engine_);

            if (data.header_data_template)
            {
                const std::vector<std::string> header =
                    resolve_table_header(*data.header_data_template, *template_engine_);
                if (header.size() != matrix.front().size())
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
                    header_row.push_back(build_title_cell(title_data, *template_engine_));
                }
                table->add_row(header_row);
            }
            else if (!data.header_titles.empty())
            {
                // An explicit <THead> can be used instead of the `header` attribute
                // alongside a JSON/template `model` -- the parser doesn't reject that
                // combination (only `header` + <THead> together is rejected), so it must
                // be honored here too, not just the header_data_template path above.
                if (data.header_titles.size() != matrix.front().size())
                {
                    throw docraft::exception::DataFormatException(
                        "<Table> THead column count must match 'model' column count");
                }
                std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> header_row;
                header_row.reserve(data.header_titles.size());
                for (const auto& title : data.header_titles)
                {
                    header_row.push_back(build_title_cell(title, *template_engine_));
                }
                table->add_row(header_row);
            }

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
                header_row.push_back(build_title_cell(title, *template_engine_));
            }
            table->add_row(header_row);
        }

        for (const auto& row_data : data.rows)
        {
            std::vector<std::shared_ptr<nodes::DocraftLoomTableCell>> row;
            if (is_vertical && row_data.row_title)
            {
                row.push_back(build_title_cell(*row_data.row_title, *template_engine_));
            }
            for (const auto& cell_data : row_data.cells)
            {
                row.push_back(build_content_cell(cell_data, *template_engine_));
            }
            table->add_row(row);
        }

        apply_common_attributes(*table, element.common);
        return table;
    }

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomTreeBuilder::build_layout(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedLayoutData&>(element.data);

        // Precedence: an explicit `<layout weights="...">` attribute wins over per-child
        // `weight` attributes; if neither is present, the weights vector stays empty so
        // today's default homogeneous-shrink-to-fit behavior is preserved.
        std::vector<float> weights;
        if (data.weights)
        {
            weights = *data.weights;
        }
        else
        {
            const bool any_child_weight = std::any_of(
                element.children.begin(), element.children.end(),
                [](const auto& child) { return child->common.weight.has_value(); });
            if (any_child_weight)
            {
                weights.reserve(element.children.size());
                for (const auto& child : element.children)
                {
                    weights.push_back(child->common.weight.value_or(1.0F));
                }
            }
        }

        if (data.orientation == parser::ParsedLayoutOrientation::kHorizontal)
        {
            auto node = std::make_shared<nodes::DocraftLoomHStack>();
            if (data.spacing)
            {
                node->set_spacing(*data.spacing);
            }
            if (!weights.empty())
            {
                node->set_weights(weights);
            }
            apply_common_attributes(*node, element.common);
            add_children(node, element.children);
            return node;
        }
        auto node = std::make_shared<nodes::DocraftLoomVStack>();
        if (data.spacing)
        {
            node->set_spacing(*data.spacing);
        }
        // VStack has no set_weights() yet (Phase 6) -- any collected weights are
        // deliberately not applied here.
        apply_common_attributes(*node, element.common);
        add_children(node, element.children);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomParagraph> DocraftLoomTreeBuilder::build_paragraph(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedParagraphData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomParagraph>();
        if (data.line_spacing)
        {
            node->set_line_spacing(*data.line_spacing);
        }
        if (data.space_before)
        {
            node->set_space_before(*data.space_before);
        }
        if (data.space_after)
        {
            node->set_space_after(*data.space_after);
        }
        if (data.alignment)
        {
            node->set_alignment(to_loom_alignment(*data.alignment));
        }
        apply_common_attributes(*node, element.common);
        // Bare PCDATA directly inside <Paragraph>text</Paragraph> is invisible to
        // add_children() below (it only walks XML element children, see ParsedElement's
        // recursion) -- surface it here as an implicit Text child so a Paragraph with no
        // explicit <Text> wrapper still renders, matching the natural way authors write
        // Craft Language paragraphs.
        if (!data.text.empty())
        {
            auto text_node = std::make_shared<nodes::DocraftLoomText>();
            text_node->set_text(template_engine_->render_template_string(data.text, current_foreach_item_));
            node->add_child(text_node);
        }
        add_children(node, element.children);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomVStack> DocraftLoomTreeBuilder::build_section(const ParsedElement& element)
    {
        const auto& data = std::any_cast<const parser::ParsedSectionData&>(element.data);
        // Header/Body/Footer only ever use background_color/border_color/border_width
        // from ParsedSectionData -- never Rectangle's width_/height_/padding_ -- so a
        // VStack (with its own composed DocraftLoomShapeStyle, see
        // DocraftLoomVStack::style()) is a closer fit than Rectangle and reuses the
        // vertical-stacking flow instead of duplicating it.
        auto node = std::make_shared<nodes::DocraftLoomVStack>();
        if (data.background_color)
        {
            node->edit_style().background_color = *data.background_color;
        }
        if (data.border_color)
        {
            node->edit_style().border_color = *data.border_color;
        }
        if (data.border_width)
        {
            node->edit_style().border_width = *data.border_width;
        }
        // Margins (margin_top/bottom/left/right) are not a rectangle concept -- they are
        // consumed directly from ParsedSectionData by DocraftLoomCraftLanguageParser to
        // build a DocraftLoomPdfCreator::Margins, not applied here.
        apply_common_attributes(*node, element.common);
        add_children(node, element.children);
        return node;
    }
} // namespace docraft::loom::craft

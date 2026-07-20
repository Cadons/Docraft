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

#include "docraft/craft/parser/docraft_parser.h"

#include <optional>

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/exception/docraft_exceptions.h"

namespace docraft::craft::parser {
    namespace {
        ParsedTextAlignment parse_table_title_alignment(const pugi::xml_node& title)
        {
            if (auto alignment_attr = title.attribute(elements::table_title::attribute::kAlignment.data()))
            {
                const std::string alignment_str = alignment_attr.as_string();
                if (alignment_str == std::string{alignment::kLeft}) {
                    return ParsedTextAlignment::kLeft;
                }
                if (alignment_str == std::string{alignment::kRight}) {
                    return ParsedTextAlignment::kRight;
                }
                if (alignment_str == std::string{alignment::kJustified}) {
                    return ParsedTextAlignment::kJustified;
                }
                if (alignment_str == std::string{alignment::kCenter}) {
                    return ParsedTextAlignment::kCenter;
                }
                throw docraft::exception::InvalidInputException("Invalid table title alignment: " + alignment_str);
            }
            return ParsedTextAlignment::kCenter;
        }

        ParsedTextStyle parse_table_title_style(const pugi::xml_node& title)
        {
            if (auto style_attr = title.attribute(elements::table_title::attribute::kStyle.data()))
            {
                const std::string style_str = style_attr.as_string();
                if (style_str == std::string{style::kBold}) {
                    return ParsedTextStyle::kBold;
                }
                if (style_str == std::string{style::kItalic}) {
                    return ParsedTextStyle::kItalic;
                }
                if (style_str == std::string{style::kBoldItalic}) {
                    return ParsedTextStyle::kBoldItalic;
                }
                if (style_str == std::string{style::kNormal}) {
                    return ParsedTextStyle::kNormal;
                }
                throw docraft::exception::InvalidInputException("Invalid table title style: " + style_str);
            }
            return ParsedTextStyle::kBold;
        }

        ParsedTableTitleData parse_table_title(const pugi::xml_node& title, const char* background_attr_name)
        {
            ParsedTableTitleData data;
            data.text = title.child_value();
            data.alignment = parse_table_title_alignment(title);
            data.style = parse_table_title_style(title);
            if (auto color_attr = title.attribute(elements::table_title::attribute::kColor.data()))
            {
                data.color = detail::get_color_attribute_raw(color_attr);
            }
            if (auto bg_attr = title.attribute(background_attr_name))
            {
                data.background = detail::get_color_attribute_raw(bg_attr);
            }
            return data;
        }

        std::optional<std::string> parse_background_color(const pugi::xml_node& node, const char* attr_name)
        {
            if (auto attr = node.attribute(attr_name))
            {
                return detail::get_color_attribute_raw(attr);
            }
            return std::nullopt;
        }

        ParsedTableCellData parse_cell(const pugi::xml_node& cell)
        {
            ParsedTableCellData data;
            if (auto width_attr = cell.attribute(basic::attribute::kWidth.data()))
            {
                const float explicit_width = width_attr.as_float();
                if (explicit_width <= 0.0F) {
                    throw docraft::exception::InvalidInputException("Cell width must be > 0");
                }
                data.width = explicit_width;
            }
            data.background = parse_background_color(cell, elements::table_column::attribute::kBackgroundColor.data());

            const pugi::xml_node child = cell.first_child();
            const std::string child_name = child.name();
            data.content_common = detail::parse_common_node_attributes(child);
            if (child_name == std::string{elements::kText})
            {
                DocraftTextParser text_parser;
                data.content_tag_name = std::string{elements::kText};
                data.content = text_parser.parse(child);
            }
            else if (child_name == std::string{elements::kImage})
            {
                DocraftImageParser image_parser;
                data.content_tag_name = std::string{elements::kImage};
                data.content = image_parser.parse(child);
            }
            return data;
        }

        void parse_thead(const pugi::xml_node& thead, ParsedTableData& table_data)
        {
            for (auto title : thead.children())
            {
                const std::string title_name = title.name();
                if (title_name == std::string{elements::kHTitle})
                {
                    table_data.header_titles.push_back(
                        parse_table_title(title, elements::table_htitle::attribute::kBackgroundColor.data()));
                    continue;
                }
                if (title_name == std::string{elements::kTitle})
                {
                    throw docraft::exception::InvalidInputException(
                        "Title is reserved for text headings; use HTitle in table headers");
                }
                throw docraft::exception::InvalidInputException(title_name + " cannot be placed in a table header");
            }
        }

        ParsedTableRowData parse_horizontal_row(const pugi::xml_node& row)
        {
            ParsedTableRowData row_data;
            row_data.background = parse_background_color(row, elements::table_row::attribute::kBackgroundColor.data());
            for (auto col: row.children()) {
                if (col.name() != std::string{elements::kCell})
                {
                    throw docraft::exception::InvalidInputException(
                        std::string(col.name()) + " is not supported in the table body");
                }
                row_data.cells.push_back(parse_cell(col));
            }
            return row_data;
        }

        ParsedTableRowData parse_vertical_row(const pugi::xml_node& row)
        {
            ParsedTableRowData row_data;
            row_data.background = parse_background_color(row, elements::table_row::attribute::kBackgroundColor.data());
            bool found_vtitle = false;
            for (auto col: row.children()) {
                const std::string col_name = col.name();
                if (col_name == std::string{elements::kVTitle}) {
                    if (found_vtitle)
                    {
                        throw docraft::exception::InvalidInputException("Only one VTitle is allowed per Row");
                    }
                    found_vtitle = true;
                    row_data.row_title = parse_table_title(
                        col, elements::table_vtitle::attribute::kBackgroundColor.data());
                    continue;
                }
                if (col_name == std::string{elements::kCell}) {
                    row_data.cells.push_back(parse_cell(col));
                    continue;
                }
                throw docraft::exception::InvalidInputException(col_name + " is not supported in the table body");
            }
            if (!found_vtitle) {
                throw docraft::exception::InvalidInputException("VTitle is mandatory for vertical table rows");
            }
            return row_data;
        }

        void parse_tbody(const pugi::xml_node& tbody, ParsedTableData& table_data, bool is_vertical)
        {
            for (auto row : tbody.children())
            {
                if (row.name() != std::string{elements::kRow}) {
                    throw docraft::exception::InvalidInputException(
                        std::string(row.name()) + " cannot be placed in a table body");
                }
                table_data.rows.push_back(is_vertical ? parse_vertical_row(row) : parse_horizontal_row(row));
            }
        }
    } // namespace

    std::any DocraftTableParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedTableData data;

        if (auto baseline_attr = craft_language_source.attribute(elements::table::attribute::kBaselineOffset.data()))
        {
            data.baseline_offset = baseline_attr.as_float();
        }

        // "horizontal"/"vertical" set orientation directly; any other value is a JSON
        // matrix literal or a `${...}` expression resolving to one -- stored verbatim and
        // resolved by the loom builder (${...} substitution, then JSON-parse), mirroring
        // how `<Foreach model="...">` defers resolution.
        if (auto model_attr = craft_language_source.attribute(elements::table::attribute::kModel.data()))
        {
            const std::string model_str = model_attr.as_string();
            if (model_str == std::string{orientation::kVertical})
            {
                data.orientation = ParsedTableOrientation::kVertical;
            }
            else if (model_str == std::string{orientation::kHorizontal})
            {
                data.orientation = ParsedTableOrientation::kHorizontal;
            }
            else
            {
                data.model_data_template = model_str;
            }
        }

        if (auto header_attr = craft_language_source.attribute(elements::table::attribute::kHeader.data()))
        {
            data.header_data_template = header_attr.as_string();
        }

        if (auto tile_attr = craft_language_source.attribute(elements::table::attribute::kTableTile.data()))
        {
            data.default_cell_background = detail::get_color_attribute_raw(tile_attr);
        }

        const bool is_vertical = data.orientation == ParsedTableOrientation::kVertical;
        const bool has_json_model = data.model_data_template.has_value();

        if (has_json_model && is_vertical)
        {
            throw docraft::exception::InvalidInputException(
                "Table 'model' JSON/template data is not supported with vertical orientation");
        }

        if (auto thead = craft_language_source.child(elements::kTHead.data()))
        {
            if (data.header_data_template)
            {
                throw docraft::exception::InvalidInputException(
                    "Table 'header' attribute cannot be combined with an explicit THead");
            }
            parse_thead(thead, data);
        }
        else if (!is_vertical && !has_json_model)
        {
            throw docraft::exception::InvalidInputException(
                std::string(elements::kTHead) + " tag not found, it is mandatory");
        }

        // An explicit TBody alongside a JSON/template `model` is only meaningful when the
        // model resolves to an array of objects, in which case the loom builder uses these
        // rows as a per-object template (mirroring `<Foreach>`'s model+children expansion).
        // Whether the model is an object array or a string matrix is only known once it's
        // resolved (it may itself be a `${...}` expression), so that distinction -- and
        // rejecting a string-matrix model paired with an explicit TBody -- is enforced by
        // the loom builder instead of here.
        if (auto tbody = craft_language_source.child(elements::kTBody.data()))
        {
            parse_tbody(tbody, data, is_vertical);
        }

        return data;
    }
} // namespace docraft::craft::parser

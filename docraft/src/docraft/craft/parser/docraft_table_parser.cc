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
#include "docraft/model/docraft_image.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_text.h"

namespace docraft::craft::parser {
    namespace {
        struct TableParseState {
            bool has_model_json = false;
            bool has_model_template = false;
            bool has_header_json = false;
            bool has_header_template = false;
            bool require_body = false;
        };

        pugi::xml_attribute get_attribute(const pugi::xml_node &node, std::string_view name);

        pugi::xml_node get_child(const pugi::xml_node &node, std::string_view name);

        model::TextAlignment parse_table_title_alignment(const pugi::xml_node &title) {
            if (auto alignment_attr = get_attribute(title, elements::table_title::attribute::kAlignment)) {
                const std::string alignment_str = alignment_attr.as_string();
                if (alignment_str == std::string{alignment::kLeft}) {
                    return model::TextAlignment::kLeft;
                }
                if (alignment_str == std::string{alignment::kRight}) {
                    return model::TextAlignment::kRight;
                }
                if (alignment_str == std::string{alignment::kJustified}) {
                    return model::TextAlignment::kJustified;
                }
                if (alignment_str == std::string{alignment::kCenter}) {
                    return model::TextAlignment::kCenter;
                }
                throw docraft::exception::InvalidInputException("Invalid table title alignment: " + alignment_str);
            }
            return model::TextAlignment::kCenter;
        }

        model::TextStyle parse_table_title_style(const pugi::xml_node &title) {
            if (auto style_attr = get_attribute(title, elements::table_title::attribute::kStyle)) {
                const std::string style_str = style_attr.as_string();
                if (style_str == std::string{style::kBold}) {
                    return model::TextStyle::kBold;
                }
                if (style_str == std::string{style::kItalic}) {
                    return model::TextStyle::kItalic;
                }
                if (style_str == std::string{style::kBoldItalic}) {
                    return model::TextStyle::kBoldItalic;
                }
                if (style_str == std::string{style::kNormal}) {
                    return model::TextStyle::kNormal;
                }
                throw docraft::exception::InvalidInputException("Invalid table title style: " + style_str);
            }
            return model::TextStyle::kBold;
        }

        std::shared_ptr<model::DocraftText> parse_table_title_node(const pugi::xml_node &title) {
            auto title_node = std::make_shared<model::DocraftText>(title.child_value());
            title_node->set_alignment(parse_table_title_alignment(title));
            title_node->set_style(parse_table_title_style(title));
            if (auto color_attr = get_attribute(title, elements::table_title::attribute::kColor)) {
                title_node->set_color(detail::get_docraft_color(color_attr));
            }
            return title_node;
        }

        std::optional<DocraftColor> parse_background_color(const pugi::xml_node &node,
                                                           const char *primary_attr,
                                                           const char *alt_attr = nullptr) {
            if (auto attr = node.attribute(primary_attr)) {
                return detail::get_docraft_color(attr);
            }
            if (alt_attr) {
                if (auto attr = node.attribute(alt_attr)) {
                    return detail::get_docraft_color(attr);
                }
            }
            return std::nullopt;
        }

        pugi::xml_attribute get_attribute(const pugi::xml_node &node, std::string_view name) {
            return node.attribute(std::string(name).c_str());
        }

        pugi::xml_node get_child(const pugi::xml_node &node, std::string_view name) {
            return node.child(std::string(name).c_str());
        }

        void parse_table_model_value(const std::shared_ptr<model::DocraftTable> &table_node,
                                     TableParseState &state,
                                     const std::string &model_str) {
            if (model_str == std::string{orientation::kVertical}) {
                table_node->set_orientation(model::LayoutOrientation::kVertical);
                return;
            }
            if (model_str == std::string{orientation::kHorizontal}) {
                table_node->set_orientation(model::LayoutOrientation::kHorizontal);
                return;
            }

            if (model_str.contains("${")) {
                state.has_model_template = true;
                table_node->set_model_template(model_str);
                return;
            }

            const auto model_type = model::DocraftTable::identify_model_type(model_str);
            state.has_model_json = true;
            if (model_type == model::DocraftModelType::kStringMatrix) {
                table_node->set_model_type(model::DocraftModelType::kStringMatrix);
                table_node->apply_json_rows(model_str);
            } else if (model_type == model::DocraftModelType::kJsonObject) {
                table_node->set_model_type(model::DocraftModelType::kJsonObject);
                state.require_body = true;
            }
            table_node->set_model_template(model_str);
        }

        void parse_table_model_attribute(const pugi::xml_node &craft_language_source,
                                         const std::shared_ptr<model::DocraftTable> &table_node,
                                         TableParseState &state) {
            if (auto model_attr = get_attribute(craft_language_source, elements::table::attribute::kModel)) {
                parse_table_model_value(table_node, state, model_attr.as_string());
            }
        }

        void parse_table_header_attribute(const pugi::xml_node &craft_language_source,
                                          const std::shared_ptr<model::DocraftTable> &table_node,
                                          TableParseState &state) {
            if (auto header_attr = get_attribute(craft_language_source, "header")) {
                const std::string header_str = header_attr.as_string();
                if (header_str.contains("${")) {
                    table_node->set_header_template(header_str);
                    state.has_header_template = true;
                } else {
                    table_node->apply_json_header(header_str);
                    state.has_header_json = true;
                }
            }
        }

        void apply_width_if_present(const pugi::xml_node &col, const std::shared_ptr<model::DocraftNode> &node) {
            if (auto width_attr = get_attribute(col, basic::attribute::kWidth)) {
                const float explicit_width = width_attr.as_float();
                if (explicit_width <= 0.0F) {
                    throw docraft::exception::InvalidInputException("Cell width must be > 0");
                }
                node->set_width(explicit_width);
            }
        }

        void append_table_cell_node(const pugi::xml_node &col,
                                    const std::shared_ptr<model::DocraftTable> &table_node,
                                    const std::shared_ptr<model::DocraftNode> &node) {
            const auto cell_bg = parse_background_color(
                col,
                std::string(elements::table_column::attribute::kBackgroundColor).c_str(),
                std::string(elements::table_column::attribute::kTableTile).c_str());
            table_node->add_content_node(node, cell_bg);
        }

        std::shared_ptr<model::DocraftNode> parse_table_cell_node(const pugi::xml_node &child) {
            if (child.name() == std::string{elements::kText}) {
                DocraftTextParser text_parser;
                return text_parser.parse(child);
            }
            if (child.name() == std::string{elements::kImage}) {
                DocraftImageParser image_parser;
                return image_parser.parse(child);
            }
            return nullptr;
        }

        void parse_supported_table_cell(const pugi::xml_node &col,
                                        const std::shared_ptr<model::DocraftTable> &table_node,
                                        int &row_value_cols) {
            if (col.children().empty()) {
                return;
            }

            auto child = col.first_child();
            auto node = parse_table_cell_node(child);
            if (!node) {
                throw docraft::exception::InvalidInputException(std::string(child.name()) +
                                                                " is not supported in the table column");
            }
            apply_width_if_present(col, node);
            append_table_cell_node(col, table_node, node);
            row_value_cols++;
        }

        void parse_vertical_body_row_title(const pugi::xml_node &col,
                                           const std::shared_ptr<model::DocraftTable> &table_node,
                                           std::vector<std::string> &v_titles,
                                           bool &found_vtitle) {
            if (found_vtitle) {
                throw docraft::exception::InvalidInputException("Only one VTitle is allowed per Row");
            }
            found_vtitle = true;
            auto title_node = parse_table_title_node(col);
            const auto bg = parse_background_color(
                col,
                std::string(elements::table_vtitle::attribute::kBackgroundColor).c_str());
            table_node->add_title_node(title_node, bg);
            v_titles.emplace_back(col.child_value());
        }

        void parse_table_body_cell(const pugi::xml_node &col,
                                   const std::shared_ptr<model::DocraftTable> &table_node,
                                   int &row_value_cols) {
            parse_supported_table_cell(col, table_node, row_value_cols);
        }

        int parse_horizontal_table_body_row(const pugi::xml_node &row,
                                            const std::shared_ptr<model::DocraftTable> &table_node) {
            const auto row_bg = parse_background_color(
                row,
                std::string(elements::table_row::attribute::kBackgroundColor).c_str());
            table_node->add_row_background(row_bg);

            int row_value_cols = 0;
            for (auto col: row.children()) {
                if (col.name() == std::string{elements::kCell}) {
                    parse_table_body_cell(col, table_node, row_value_cols);
                    continue;
                }
                throw docraft::exception::InvalidInputException(std::string(col.name()) +
                                                                " is not supported in the table body");
            }
            return row_value_cols;
        }

        int parse_vertical_table_body_row(const pugi::xml_node &row,
                                          const std::shared_ptr<model::DocraftTable> &table_node,
                                          std::vector<std::string> &v_titles) {
            const auto row_bg = parse_background_color(
                row,
                std::string(elements::table_row::attribute::kBackgroundColor).c_str());
            table_node->add_row_background(row_bg);

            int row_value_cols = 0;
            bool found_vtitle = false;
            for (auto col: row.children()) {
                const std::string col_name = col.name();
                if (col_name == std::string{elements::kVTitle}) {
                    parse_vertical_body_row_title(col, table_node, v_titles, found_vtitle);
                    continue;
                }
                if (col_name == std::string{elements::kCell}) {
                    parse_table_body_cell(col, table_node, row_value_cols);
                    continue;
                }
                throw docraft::exception::InvalidInputException(std::string(col.name()) +
                                                                " is not supported in the table body");
            }

            if (!found_vtitle) {
                throw docraft::exception::InvalidInputException("VTitle is mandatory for vertical table rows");
            }
            return row_value_cols;
        }

        void append_vertical_header_title(const pugi::xml_node &title,
                                          const std::shared_ptr<model::DocraftTable> &table_node) {
            auto title_node = parse_table_title_node(title);
            const auto bg = parse_background_color(
                title,
                std::string(elements::table_htitle::attribute::kBackgroundColor).c_str());
            table_node->add_htitle_node(title_node, bg);
        }

        void append_horizontal_header_title(const pugi::xml_node &title,
                                            const std::shared_ptr<model::DocraftTable> &table_node,
                                            std::vector<std::string> &titles) {
            auto title_node = parse_table_title_node(title);
            const auto bg = parse_background_color(
                title,
                std::string(elements::table_htitle::attribute::kBackgroundColor).c_str());
            table_node->add_title_node(title_node, bg);
            titles.emplace_back(title.child_value());
        }

        void parse_vertical_table_header(const pugi::xml_node &table_header,
                                         const std::shared_ptr<model::DocraftTable> &table_node) {
            int header_cols = 0;
            for (auto title: table_header.children()) {
                if (title.name() == std::string{elements::kHTitle}) {
                    header_cols++;
                    append_vertical_header_title(title, table_node);
                    continue;
                }
                if (title.name() == std::string{elements::kTitle}) {
                    throw docraft::exception::InvalidInputException(
                        "Title is reserved for text headings; use HTitle in table headers");
                }
                throw docraft::exception::InvalidInputException(
                    std::string(title.name()) + " cannot be placed in a table header");
            }
            if (header_cols > 0) {
                table_node->set_content_cols(header_cols);
                table_node->set_cols(header_cols + 1);
            }
        }

        void parse_horizontal_table_header(const pugi::xml_node &table_header,
                                           const std::shared_ptr<model::DocraftTable> &table_node,
                                           const TableParseState &state) {
            int col_number = 0;
            const int existing_cols = table_node->content_cols();
            std::vector<std::string> titles;
            for (auto title: table_header.children()) {
                if (title.name() == std::string{elements::kHTitle}) {
                    col_number++;
                    append_horizontal_header_title(title, table_node, titles);
                    continue;
                }
                if (title.name() == std::string{elements::kTitle} || title.name() == std::string{elements::kVTitle}) {
                    throw docraft::exception::InvalidInputException(
                        "Use HTitle in table headers (VTitle is only for vertical row labels)");
                }
                throw docraft::exception::InvalidInputException(
                    std::string(title.name()) + " cannot be placed in a table header");
            }

            if ((state.has_model_json || state.has_model_template) && existing_cols > 0 && existing_cols !=
                col_number) {
                throw docraft::exception::InvalidInputException("Table header columns do not match model columns");
            }
            table_node->set_titles(titles);
            table_node->set_cols(col_number);
            table_node->set_content_cols(col_number);
        }

        TableParseState parse_table_model_and_header_attributes(const pugi::xml_node &craft_language_source,
                                                                const std::shared_ptr<model::DocraftTable> &
                                                                table_node) {
            TableParseState state;
            parse_table_model_attribute(craft_language_source, table_node, state);
            parse_table_header_attribute(craft_language_source, table_node, state);

            return state;
        }

        void validate_table_model_header_constraints(const TableParseState &state,
                                                     const std::shared_ptr<model::DocraftTable> &table_node,
                                                     const pugi::xml_node &table_header,
                                                     const pugi::xml_node &table_body) {
            // JSON/templated data is mutually exclusive with explicit body nodes.
            // `THead` can be used instead of `header`, but not together.
            if (state.has_model_json || state.has_model_template || state.has_header_json || state.
                has_header_template) {
                if (table_body) {
                    //Tbody is template for each item
                }
                if ((state.has_header_json || state.has_header_template) && table_header) {
                    throw docraft::exception::InvalidInputException("Table JSON header cannot be combined with THead");
                }
                if (table_node->orientation() == model::LayoutOrientation::kVertical) {
                    throw docraft::exception::InvalidInputException("Table JSON model does not support vertical model");
                }
            }
        }


        void parse_table_cell_content(const pugi::xml_node &col,
                                      const std::shared_ptr<model::DocraftTable> &table_node,
                                      int &row_value_cols) {
            parse_supported_table_cell(col, table_node, row_value_cols);
        }

        int parse_table_body_row(const pugi::xml_node &row,
                                 const std::shared_ptr<model::DocraftTable> &table_node,
                                 const bool is_vertical,
                                 std::vector<std::string> &v_titles) {
            if (is_vertical) {
                return parse_vertical_table_body_row(row, table_node, v_titles);
            }
            return parse_horizontal_table_body_row(row, table_node);
        }

        void finalize_table_body(const std::shared_ptr<model::DocraftTable> &table_node,
                                 const bool is_vertical,
                                 const int row_count,
                                 const int max_value_cols,
                                 const std::vector<std::string> &v_titles) {
            if (is_vertical) {
                if (!v_titles.empty()) {
                    table_node->set_titles(v_titles);
                }
                if (table_node->content_cols() <= 0 && max_value_cols > 0) {
                    table_node->set_content_cols(max_value_cols);
                }
                if (table_node->cols() <= 0) {
                    table_node->set_cols(std::max(2, table_node->content_cols() + 1));
                }
            }
            table_node->set_rows(row_count);
        }

        void parse_explicit_table_body(const pugi::xml_node &table_body,
                                       const std::shared_ptr<model::DocraftTable> &table_node,
                                       const bool is_vertical) {
            int row_count = 0;
            int max_value_cols = 0;
            std::vector<std::string> v_titles;
            for (auto row: table_body.children()) {
                if (row.name() != std::string{elements::kRow}) {
                    throw docraft::exception::InvalidInputException(
                        std::string(row.name()) + " cannot be placed in a table body");
                }
                row_count++;

                const int row_value_cols = parse_table_body_row(row, table_node, is_vertical, v_titles);
                max_value_cols = std::max(max_value_cols, row_value_cols);
            }

            finalize_table_body(table_node, is_vertical, row_count, max_value_cols, v_titles);
        }
    } // namespace

    std::shared_ptr<model::DocraftNode> DocraftTableParser::parse(const pugi::xml_node &craft_language_source) {
        auto table_node = std::make_shared<model::DocraftTable>();

        // Baseline tweak for text vertical alignment inside cells.
        if (auto baseline_attr = get_attribute(craft_language_source, elements::table::attribute::kBaselineOffset)) {
            table_node->set_baseline_offset(baseline_attr.as_float());
        }

        const TableParseState state = parse_table_model_and_header_attributes(craft_language_source, table_node);

        if (auto tile_attr = get_attribute(craft_language_source, elements::table::attribute::kTableTile)) {
            table_node->set_default_cell_background(detail::get_docraft_color(tile_attr));
        }

        const bool is_vertical = table_node->orientation() == model::LayoutOrientation::kVertical;
        auto table_header = get_child(craft_language_source, elements::kTHead);
        auto table_body = get_child(craft_language_source, elements::kTBody);

        validate_table_model_header_constraints(state, table_node, table_header, table_body);

        if (table_header) {
            if (is_vertical) {
                parse_vertical_table_header(table_header, table_node);
            } else {
                parse_horizontal_table_header(table_header, table_node, state);
            }
        } else if (!is_vertical && !state.has_model_json && !state.has_model_template && !state.has_header_json && !
                   state.has_header_template) {
            throw exception::InvalidInputException(std::string(elements::kTHead) +
                                                   " tag not found, it is mandatory");
        }

        if (table_body) {
            parse_explicit_table_body(table_body, table_node, is_vertical);
        }
        if (state.require_body) {
            table_node->apply_json_rows(table_node->model_template());
        }
        detail::configure_docraft_node_attributes(table_node, craft_language_source);
        return table_node;
    }
}

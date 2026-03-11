#include "docraft/craft/parser/docraft_parser.h"

#include <optional>

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_image.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_text.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftTableParser::parse(const pugi::xml_node &craft_language_source) {
        auto table_node = std::make_shared<model::DocraftTable>();

        // Baseline tweak for text vertical alignment inside cells.
        if (auto baseline_attr = craft_language_source.attribute(elements::table::attribute::kBaselineOffset.data())) {
            table_node->set_baseline_offset(baseline_attr.as_float());
        }

        // `model` is overloaded:
        // - "horizontal"/"vertical" -> orientation only
        // - JSON matrix -> data rows (horizontal only)
        // - ${var} -> deferred JSON rows (templated)
        bool has_model_json = false;
        bool has_model_template = false;
        bool has_header_json = false;
        bool has_header_template = false;
        bool require_body = false;
        if (auto model_attr = craft_language_source.attribute(elements::table::attribute::kModel.data())) {
            std::string model_str = model_attr.as_string();
            if (model_str == std::string{orientation::kVertical}) {
                table_node->set_orientation(model::LayoutOrientation::kVertical);
            } else if (model_str == std::string{orientation::kHorizontal}) {
                table_node->set_orientation(model::LayoutOrientation::kHorizontal);
            } else {
                if (model_str.contains("${")) {
                    has_model_template = true;
                } else {
                    auto model_type = model::DocraftTable::identify_model_type(model_str);
                    has_model_json = true;
                    if (model_type == model::DocraftModelType::kStringMatrix) {
                        table_node->set_model_type(model::DocraftModelType::kStringMatrix);
                        table_node->apply_json_rows(model_str);
                    } else if (model_type == model::DocraftModelType::kJsonObject) {
                        table_node->set_model_type(model::DocraftModelType::kJsonObject);
                        require_body = true;
                    }
                }
                table_node->set_model_template(model_str);

            }
        }
        // Optional `header` attribute for column titles:
        // - JSON array -> titles
        // - ${var} -> deferred JSON titles (templated)
        if (auto header_attr = craft_language_source.attribute("header")) {
            std::string header_str = header_attr.as_string();
            if (header_str.contains("${")) {
                table_node->set_header_template(header_str);
                has_header_template = true;
            } else {
                table_node->apply_json_header(header_str);
                has_header_json = true;
            }
        }

        if (auto tile_attr = craft_language_source.attribute(elements::table::attribute::kTableTile.data())) {
            table_node->set_default_cell_background(detail::get_docraft_color(tile_attr));
        }

        auto parse_title_node = [&](const pugi::xml_node &title) -> std::shared_ptr<model::DocraftText> {
            auto title_node = std::make_shared<model::DocraftText>(title.child_value());
            if (auto alignment_attr = title.attribute(elements::table_title::attribute::kAlignment.data())) {
                std::string alignment_str = alignment_attr.as_string();
                if (alignment_str == std::string{alignment::kLeft}) {
                    title_node->set_alignment(model::TextAlignment::kLeft);
                } else if (alignment_str == std::string{alignment::kRight}) {
                    title_node->set_alignment(model::TextAlignment::kRight);
                } else if (alignment_str == std::string{alignment::kJustified}) {
                    title_node->set_alignment(model::TextAlignment::kJustified);
                } else if (alignment_str == std::string{alignment::kCenter}) {
                    title_node->set_alignment(model::TextAlignment::kCenter);
                } else {
                    throw std::invalid_argument("Invalid table title alignment: " + alignment_str);
                }
            } else {
                title_node->set_alignment(model::TextAlignment::kCenter);
            }
            if (auto style_attr = title.attribute(elements::table_title::attribute::kStyle.data())) {
                std::string style_str = style_attr.as_string();
                if (style_str == std::string{style::kBold}) {
                    title_node->set_style(model::TextStyle::kBold);
                } else if (style_str == std::string{style::kItalic}) {
                    title_node->set_style(model::TextStyle::kItalic);
                } else if (style_str == std::string{style::kBoldItalic}) {
                    title_node->set_style(model::TextStyle::kBoldItalic);
                } else if (style_str == std::string{style::kNormal}) {
                    title_node->set_style(model::TextStyle::kNormal);
                } else {
                    throw std::invalid_argument("Invalid table title style: " + style_str);
                }
            } else {
                title_node->set_style(model::TextStyle::kBold);
            }
            if (auto color_attr = title.attribute(elements::table_title::attribute::kColor.data())) {
                title_node->set_color(detail::get_docraft_color(color_attr));
            }
            return title_node;
        };

        auto parse_background_color = [&](const pugi::xml_node &node,
                                          const char *primary_attr,
                                          const char *alt_attr = nullptr)
            -> std::optional<DocraftColor> {
            if (auto attr = node.attribute(primary_attr)) {
                return detail::get_docraft_color(attr);
            }
            if (alt_attr) {
                if (auto attr = node.attribute(alt_attr)) {
                    return detail::get_docraft_color(attr);
                }
            }
            return std::nullopt;
        };

        const bool is_vertical = table_node->orientation() == model::LayoutOrientation::kVertical;
        auto table_header = craft_language_source.child(elements::kTHead.data());
        auto table_body = craft_language_source.child(elements::kTBody.data());

        // JSON/templated data is mutually exclusive with explicit body nodes.
        // `THead` can be used instead of `header`, but not together.
        if (has_model_json || has_model_template || has_header_json || has_header_template) {
            if (table_body) {
                //Tbody is template for each item
            }
            if ((has_header_json || has_header_template) && table_header) {
                throw std::invalid_argument("Table JSON header cannot be combined with THead");
            }
            if (table_node->orientation() == model::LayoutOrientation::kVertical) {
                throw std::invalid_argument("Table JSON model does not support vertical model");
            }
        }

        // Parse explicit THead (static titles).
        if (table_header) {
            if (is_vertical) {
                int header_cols = 0;
                for (auto title: table_header.children()) {
                    if (title.name() == std::string{elements::kHTitle}) {
                        header_cols++;
                        auto title_node = parse_title_node(title);
                        const auto bg = parse_background_color(
                            title,
                            elements::table_htitle::attribute::kBackgroundColor.data());
                        table_node->add_htitle_node(title_node, bg);
                    } else if (title.name() == std::string{elements::kTitle}) {
                        throw std::invalid_argument("Title is reserved for text headings; use HTitle in table headers");
                    } else {
                        throw std::invalid_argument(std::string(title.name()) + " cannot be placed in a table header");
                    }
                }
                if (header_cols > 0) {
                    table_node->set_content_cols(header_cols);
                    table_node->set_cols(header_cols + 1);
                }
            } else {
                int col_number = 0;
                const int existing_cols = table_node->content_cols();
                std::vector<std::string> titles;
                for (auto title: table_header.children()) {
                    if (title.name() == std::string{elements::kHTitle}) {
                        col_number++;
                        auto title_node = parse_title_node(title);
                        const auto bg = parse_background_color(
                            title,
                            elements::table_htitle::attribute::kBackgroundColor.data());
                        table_node->add_title_node(title_node, bg);
                        titles.emplace_back(title.child_value());
                    } else if (title.name() == std::string{elements::kTitle} ||
                               title.name() == std::string{elements::kVTitle}) {
                        throw std::invalid_argument(
                            "Use HTitle in table headers (VTitle is only for vertical row labels)");
                    } else {
                        throw std::invalid_argument(std::string(title.name()) + " cannot be placed in a table header");
                    }
                }
                // If JSON rows were already provided, header must match column count.
                if ((has_model_json || has_model_template) && existing_cols > 0 && existing_cols != col_number) {
                    throw std::invalid_argument("Table header columns do not match model columns");
                }
                table_node->set_titles(titles);
                table_node->set_cols(col_number);
                table_node->set_content_cols(col_number);
            }
        } else if (!is_vertical && !has_model_json && !has_model_template && !has_header_json && !has_header_template) {
            throw std::invalid_argument(std::string(elements::kTHead.data()) +
                                        " tag not found, it is mandatory");
        }

        // Parse body rows when using explicit TBody.
        table_body = craft_language_source.child(elements::kTBody.data());
        if (table_body) {
            int row_count = 0;
            int max_value_cols = 0;
            std::vector<std::string> v_titles;
            for (auto row: table_body.children()) {
                if (row.name() != std::string{elements::kRow}) {
                    throw std::invalid_argument(std::string(row.name()) + " cannot be placed in a table body");
                }

                const auto row_bg = parse_background_color(
                    row,
                    elements::table_row::attribute::kBackgroundColor.data());
                table_node->add_row_background(row_bg);
                row_count++;

                int row_value_cols = 0;
                bool found_vtitle = false;
                for (auto col: row.children()) {
                    const std::string col_name = col.name();
                    if (is_vertical && col_name == std::string{elements::kVTitle}) {
                        if (found_vtitle) {
                            throw std::invalid_argument("Only one VTitle is allowed per Row");
                        }
                        found_vtitle = true;
                        auto title_node = parse_title_node(col);
                        const auto bg = parse_background_color(
                            col,
                            elements::table_vtitle::attribute::kBackgroundColor.data());
                        table_node->add_title_node(title_node, bg);
                        v_titles.emplace_back(col.child_value());
                        continue;
                    }

                    if (col_name == std::string{elements::kCell}) {
                        if (!col.children().empty()) {
                            auto child = col.first_child();
                            if (child.name() == std::string{elements::kText}) {
                                DocraftTextParser text_parser;
                                auto text_node = text_parser.parse(child);
                                if (auto width_attr = col.attribute(basic::attribute::kWidth.data())) {
                                    const float explicit_width = width_attr.as_float();
                                    if (explicit_width <= 0.0F) {
                                        throw std::invalid_argument("Cell width must be > 0");
                                    }
                                    text_node->set_width(explicit_width);
                                }
                                const auto cell_bg = parse_background_color(
                                    col,
                                    elements::table_column::attribute::kBackgroundColor.data(),
                                    elements::table_column::attribute::kTableTile.data());
                                table_node->add_content_node(text_node, cell_bg);
                                row_value_cols++;
                            } else if (child.name() == std::string{elements::kImage}) {
                                DocraftImageParser image_parser;
                                auto image = image_parser.parse(child);
                                if (auto width_attr = col.attribute(basic::attribute::kWidth.data())) {
                                    const float explicit_width = width_attr.as_float();
                                    if (explicit_width <= 0.0F) {
                                        throw std::invalid_argument("Cell width must be > 0");
                                    }
                                    image->set_width(explicit_width);
                                }
                                const auto cell_bg = parse_background_color(
                                    col,
                                    elements::table_column::attribute::kBackgroundColor.data(),
                                    elements::table_column::attribute::kTableTile.data());
                                table_node->add_content_node(image, cell_bg);
                                row_value_cols++;
                            } else {
                                throw std::runtime_error(std::string(child.name()) +
                                                         " is not supported in the table column");
                            }
                        }
                    } else if (!is_vertical && col_name == std::string{elements::kVTitle}) {
                        throw std::invalid_argument("VTitle is only allowed for vertical tables");
                    } else {
                        throw std::runtime_error(std::string(col.name()) +
                                                 " is not supported in the table body");
                    }
                }

                if (is_vertical && !found_vtitle) {
                    throw std::invalid_argument("VTitle is mandatory for vertical table rows");
                }
                max_value_cols = std::max(max_value_cols, row_value_cols);
            }

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
                table_node->set_rows(row_count);
            } else {
                table_node->set_rows(row_count);
            }
        }
        if (require_body) {
            table_node->apply_json_rows(table_node->model_template());
        }
        detail::configure_docraft_node_attributes(table_node, craft_language_source);
        return table_node;
    }
}

#include "craft/parser/docraft_parser.h"

#include <cctype>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "craft/docraft_craft_language_tokens.h"
#include "model/docraft_blank_line.h"
#include "model/docraft_header.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_image.h"
#include "model/docraft_layout.h"
#include "model/docraft_settings.h"
#include "model/docraft_table.h"
#include "model/docraft_text.h"

namespace {
    bool is_hex_color(const std::string &color) {
        if (color.size() != 7 && color.size() != 9) {
            return false;
        }
        if (color[0] != '#') {
            return false;
        }
        for (size_t i = 1; i < color.size(); ++i) {
            if (!std::isxdigit(static_cast<unsigned char>(color[i]))) {
                return false;
            }
        }
        return true;
    }

    docraft::DocraftColor get_docraft_color(const pugi::xml_attribute &color_attr) {
        std::string color_name_str = color_attr.as_string();
        if (color_name_str.empty()) {
            throw std::invalid_argument("Color attribute cannot be empty");
        }

        if (color_name_str[0] == '#') {
            if (!is_hex_color(color_name_str)) {
                throw std::invalid_argument("Invalid hex color: " + color_name_str);
            }
            return docraft::DocraftColor(color_name_str);
        }

        if (color_name_str == std::string{docraft::craft::color::kBlack}) {
            return docraft::DocraftColor(docraft::ColorName::kBlack);
        } else if (color_name_str == std::string{docraft::craft::color::kWhite}) {
            return docraft::DocraftColor(docraft::ColorName::kWhite);
        } else if (color_name_str == std::string{docraft::craft::color::kRed}) {
            return docraft::DocraftColor(docraft::ColorName::kRed);
        } else if (color_name_str == std::string{docraft::craft::color::kGreen}) {
            return docraft::DocraftColor(docraft::ColorName::kGreen);
        } else if (color_name_str == std::string{docraft::craft::color::kBlue}) {
            return docraft::DocraftColor(docraft::ColorName::kBlue);
        } else if (color_name_str == std::string{docraft::craft::color::kYellow}) {
            return docraft::DocraftColor(docraft::ColorName::kYellow);
        } else if (color_name_str == std::string{docraft::craft::color::kCyan}) {
            return docraft::DocraftColor(docraft::ColorName::kCyan);
        } else if (color_name_str == std::string{docraft::craft::color::kMagenta}) {
            return docraft::DocraftColor(docraft::ColorName::kMagenta);
        } else if (color_name_str == std::string{docraft::craft::color::kPurple}) {
            return docraft::DocraftColor(docraft::ColorName::kPurple);
        }

        throw std::invalid_argument("Unknown color: " + color_name_str);
    }

    // helper to parse external font children into a DocraftFont (limited to this translation unit)
    void add_external_fonts_from_node(const pugi::xml_node &font_node,
                                      const char *child_name,
                                      docraft::model::setting::DocraftFont &docraft_font) {
        for (auto external_font: font_node.children(child_name)) {
            docraft::model::setting::DocraftExternalFont docraft_external_font;
            docraft_external_font.path = external_font.attribute(
                docraft::craft::elements::settings::fonts::font_type::attribute::kSrc.data()).as_string();
            // Use the child node name to determine the variant (e.g. "FontBold" => "Bold")
            std::string node_type = external_font.name();
            // remove "Font" prefix if present on the child tag (e.g. FontBold -> Bold)
            const std::string font_prefix = std::string{docraft::craft::elements::settings::fonts::kFont.data()};
            if (node_type.rfind(font_prefix, 0) == 0) {
                node_type = node_type.substr(font_prefix.size());
            }
            // Compose the registered font name: baseName for normal, or baseName-Suffix for others
            if (node_type.empty() || node_type == std::string{"Normal"}) {
                docraft_external_font.name = docraft_font.name; // e.g. "Arial"
            } else {
                docraft_external_font.name = docraft_font.name + std::string{"-"} + node_type; // e.g. "Arial-Bold"
            }
             docraft_font.external_fonts.push_back(docraft_external_font);
        }
    }

    void configure_docraft_node_attributes(const std::shared_ptr<docraft::model::DocraftNode> &node,
                                           const pugi::xml_node &craft_language_source) {
        // get attributes
        if (auto x_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kX.data())) {
            node->set_position({x_attr.as_float(), node->position().y});
        }
        if (auto y_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kY.data())) {
            node->set_position({node->position().x, y_attr.as_float()});
        }
        if (auto width_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kWidth.data())) {
            node->set_width(width_attr.as_float());
        }
        if (auto height_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kHeight.data())) {
            node->set_height(height_attr.as_float());
        }
        if (auto auto_fill_height_attr = craft_language_source.attribute(
                docraft::craft::basic::attribute::kAutoFillHeight.data())) {
            node->set_auto_fill_height(auto_fill_height_attr.as_bool());
        }
        if (auto auto_fill_width_attr = craft_language_source.attribute(
                docraft::craft::basic::attribute::kAutoFillWidth.data())) {
            node->set_auto_fill_width(auto_fill_width_attr.as_bool());
        }
        if (auto padding_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kPadding.data())) {
            node->set_padding(padding_attr.as_float());
        }
        if (auto position_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kPosition.data())) {
            std::string position_str = position_attr.as_string();
            if (position_str == std::string{docraft::craft::basic::attribute::position_type::kBlock}) {
                node->set_position_mode(docraft::model::DocraftPositionType::kBlock);
            } else if (position_str == std::string{docraft::craft::basic::attribute::position_type::kAbsolute}) {
                node->set_position_mode(docraft::model::DocraftPositionType::kAbsolute);
            } else {
                throw std::invalid_argument("Invalid position value: " + position_str);
            }
        }
        if (auto weight_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kWeight.data())) {
            node->set_weight(weight_attr.as_float());
        }
    }

    void configure_section_attributes(const std::shared_ptr<docraft::model::DocraftSection> &node,
                                      const pugi::xml_node &craft_language_source) {
        // get attributes
        if (auto margin_top_attr = craft_language_source.attribute(
                docraft::craft::section::attribute::kMarginTop.data())) {
            node->set_margin_top(margin_top_attr.as_float());
        }
        if (auto margin_bottom_attr = craft_language_source.attribute(
                docraft::craft::section::attribute::kMarginBottom.data())) {
            node->set_margin_bottom(margin_bottom_attr.as_float());
        }
        if (auto margin_left_attr = craft_language_source.attribute(
                docraft::craft::section::attribute::kMarginLeft.data())) {
            node->set_margin_left(margin_left_attr.as_float());
        }
        if (auto margin_right_attr = craft_language_source.attribute(
                docraft::craft::section::attribute::kMarginRight.data())) {
            node->set_margin_right(margin_right_attr.as_float());
        }
        configure_docraft_node_attributes(node, craft_language_source);
    }
} // namespace

namespace docraft::craft::parser {
    void DocraftRectangleParser::apply_attributes_to(const std::shared_ptr<model::DocraftRectangle> &from,
                                                     const std::shared_ptr<model::DocraftRectangle> &to) {
        // apply from properties to to object
        to->set_background_color(from->background_color());
        to->set_border_color(from->border_color());
    }

    std::shared_ptr<model::DocraftNode> DocraftRectangleParser::parse(const pugi::xml_node &craft_language_source) {
        auto rectangle = std::make_shared<model::DocraftRectangle>();
        if (auto background_color_attr = craft_language_source.attribute(
                elements::rectangle::attribute::kBackgroundColor.data())) {
            rectangle->set_background_color(get_docraft_color(background_color_attr));
        }
        if (auto border_color_attr =
                    craft_language_source.attribute(elements::rectangle::attribute::kBorderColor.data())) {
            rectangle->set_border_color(get_docraft_color(border_color_attr));
        }
        configure_docraft_node_attributes(rectangle, craft_language_source);
        return rectangle;
    }

    std::shared_ptr<model::DocraftNode> DocraftHeaderParser::parse(const pugi::xml_node &craft_language_source) {
        auto header_node = std::make_shared<model::DocraftHeader>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, header_node);
        configure_section_attributes(header_node, craft_language_source);
        return header_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftBodyParser::parse(const pugi::xml_node &craft_language_source) {
        auto body_node = std::make_shared<model::DocraftBody>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, body_node);
        configure_section_attributes(body_node, craft_language_source);

        return body_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftFooterParser::parse(const pugi::xml_node &craft_language_source) {
        auto footer_node = std::make_shared<model::DocraftFooter>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, footer_node);
        configure_section_attributes(footer_node, craft_language_source);
        return footer_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftTextParser::parse(const pugi::xml_node &craft_language_source) {
        auto text_node = std::make_shared<model::DocraftText>();

        // get text content
        text_node->set_text(craft_language_source.child_value());

        // get attributes
        if (auto font_size_attr = craft_language_source.attribute(elements::text::attribute::kFontSize.data())) {
            text_node->set_font_size(font_size_attr.as_float());
        }
        if (auto font_name_attr = craft_language_source.attribute(elements::text::attribute::kFontName.data())) {
            text_node->set_font_name(font_name_attr.as_string());
        }
        if (auto color_attr = craft_language_source.attribute(basic::attribute::kColor.data())) {
            text_node->set_color(get_docraft_color(color_attr));
        }
        if (auto style_attr = craft_language_source.attribute(elements::text::attribute::kStyle.data())) {
            std::string style_str = style_attr.as_string();
            if (style_str == std::string{style::kBold}) {
                text_node->set_style(model::TextStyle::kBold);
            } else if (style_str == std::string{style::kItalic}) {
                text_node->set_style(model::TextStyle::kItalic);
            } else if (style_str == std::string{style::kBoldItalic}) {
                text_node->set_style(model::TextStyle::kBoldItalic);
            } else if (style_str == std::string{style::kNormal}) {
                text_node->set_style(model::TextStyle::kNormal);
            } else {
                throw std::invalid_argument("Invalid text style: " + style_str);
            }
        }
        if (auto alignment_attr = craft_language_source.attribute(elements::text::attribute::kAlignment.data())) {
            std::string alignment_str = alignment_attr.as_string();
            if (alignment_str == std::string{alignment::kCenter}) {
                text_node->set_alignment(model::TextAlignment::kCenter);
            } else if (alignment_str == std::string{alignment::kRight}) {
                text_node->set_alignment(model::TextAlignment::kRight);
            } else if (alignment_str == std::string{alignment::kJustified}) {
                text_node->set_alignment(model::TextAlignment::kJustified);
            } else if (alignment_str == std::string{alignment::kLeft}) {
                text_node->set_alignment(model::TextAlignment::kLeft);
            } else {
                throw std::invalid_argument("Invalid text alignment: " + alignment_str);
            }
        }
        if (auto underline_attr = craft_language_source.attribute(elements::text::attribute::kUnderline.data())) {
            text_node->set_underline(underline_attr.as_bool());
        }

        configure_docraft_node_attributes(text_node, craft_language_source);
        return text_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftImageParser::parse(const pugi::xml_node &craft_language_source) {
        auto image_node = std::make_shared<model::DocraftImage>();
        if (auto src_attr = craft_language_source.attribute(elements::image::attribute::kSrc.data())) {
            image_node->set_path(src_attr.as_string());
        }
        configure_docraft_node_attributes(image_node, craft_language_source);
        return image_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftTableParser::parse(const pugi::xml_node &craft_language_source) {
        auto table_node = std::make_shared<model::DocraftTable>();

        if (auto baseline_attr = craft_language_source.attribute(elements::table::attribute::kBaselineOffset.data())) {
            table_node->set_baseline_offset(baseline_attr.as_float());
        }

        // handle model (next)
        // check if it has TableHeader with TableTile
        if (auto table_header = craft_language_source.child(elements::kTableHeader.data())) {
            int col_number = 0;
            std::vector<std::string> titles;
            for (auto title: table_header.children()) {
                if (title.name() == std::string{elements::kTableTitle}) {
                    col_number++;
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
                        title_node->set_color(get_docraft_color(color_attr));
                    }
                    table_node->add_title_node(title_node);
                    titles.emplace_back(title.child_value());
                } else {
                    throw std::invalid_argument(std::string(title.name()) + " cannot be placed in a table header");
                }
            }

            table_node->set_titles(titles);
            table_node->set_cols(col_number);

            if (auto table_body = craft_language_source.child(elements::kTableBody.data())) {
                for (auto row: table_body.children()) {
                    if (row.name() == std::string{elements::kTableRow}) {
                        if (!row.children().empty()) {
                            const std::vector<std::string> support_type = {
                                    std::string{elements::kText},
                                    std::string{elements::kImage},
                            };

                            for (auto col: row.children()) {
                                bool supported = false;
                                for (const auto &item: support_type) {
                                    if (item == col.name()) {
                                        supported = true;
                                        break;
                                    }
                                }

                                if (supported) {
                                    if (col.name() == std::string{elements::kText}) {
                                        DocraftTextParser text_parser;
                                        auto text_node = text_parser.parse(col);
                                        table_node->add_content_node(text_node);
                                    } else if (col.name() == std::string{elements::kImage}) {
                                        DocraftImageParser image_parser;
                                        auto image = image_parser.parse(col);
                                        table_node->add_content_node(image);
                                    }
                                } else {
                                    throw std::runtime_error(std::string(col.name()) +
                                                             " is not supported in the table body");
                                }
                            }
                        }
                    } else {
                        throw std::invalid_argument(std::string(row.name()) + " cannot be placed in a table body");
                    }
                }
            }
        } else {
            throw std::invalid_argument(std::string(elements::kTableHeader.data()) +
                                        " tag not found, it is mandatory");
        }

        configure_docraft_node_attributes(table_node, craft_language_source);
        return table_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftLayoutParser::parse(const pugi::xml_node &craft_language_source) {
        auto layout_node = std::make_shared<model::DocraftLayout>();
        if (auto orientation_attr = craft_language_source.attribute(elements::layout::attribute::kOrientation.data())) {
            std::string orientation_str = orientation_attr.as_string();
            if (orientation_str == std::string{orientation::kHorizontal}) {
                layout_node->set_orientation(model::LayoutOrientation::kHorizontal);
            } else if (orientation_str == std::string{orientation::kVertical}) {
                layout_node->set_orientation(model::LayoutOrientation::kVertical);
            } else {
                throw std::invalid_argument("Invalid layout orientation: " + orientation_str);
            }
        }
        configure_docraft_node_attributes(layout_node, craft_language_source);
        return layout_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftBlackLineParser::parse(const pugi::xml_node &craft_language_source) {
        auto blank = std::make_shared<model::DocraftBlankLine>();
        configure_docraft_node_attributes(blank, craft_language_source);
        return blank;
    }


    std::shared_ptr<model::DocraftNode> DocraftSettingsParser::parse(const pugi::xml_node &craft_language_source) {
        auto settings_node = std::make_shared<model::DocraftSettings>();
        for (auto font_node: craft_language_source.children()) {
            if (font_node.name() == std::string{elements::settings::kFonts.data()}) {
                //Handle external fonts
                for (auto font: font_node.children(elements::settings::fonts::kFont.data())) {
                    model::setting::DocraftFont docraft_font;
                    if (auto name_attr = font.attribute(elements::settings::fonts::attribute::kName.data())) {
                        docraft_font.name = name_attr.as_string();
                    } else {
                        throw std::invalid_argument(std::string(elements::settings::fonts::attribute::kName.data()) +
                                                    " attribute is required for a font");
                    }
                    // parse external font children
                    add_external_fonts_from_node(font, elements::settings::fonts::font_type::kFontNormal.data(),
                                                 docraft_font);
                    add_external_fonts_from_node(font, elements::settings::fonts::font_type::kFontBold.data(),
                                                 docraft_font);
                    add_external_fonts_from_node(font, elements::settings::fonts::font_type::kFontItalic.data(),
                                                 docraft_font);
                    add_external_fonts_from_node(font,
                                                 elements::settings::fonts::font_type::kFontBoldItalic.data(),
                                                 docraft_font);
                    settings_node->add_font(docraft_font);
                }
            } else {
                throw std::invalid_argument(std::string(font_node.name()) +
                                            " is not supported in settings");
            }
        }
        return settings_node;
     }
 } // namespace docraft::craft::parser

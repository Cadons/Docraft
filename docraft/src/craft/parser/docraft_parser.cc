#include "craft/parser/docraft_parser.h"

#include <iostream>
#include <ostream>

#include "craft/docraft_craft_language_tokens.h"
#include "model/docraft_blank_line.h"
#include "model/docraft_header.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_image.h"
#include "model/docraft_layout.h"
#include "model/docraft_table.h"
#include "model/docraft_text.h"

namespace {
    docraft::DocraftColor get_docraft_color(const pugi::xml_attribute &color_attr) {
        if (color_attr.as_string()[0] == '#') {
            return docraft::DocraftColor(color_attr.as_string());
        }
        std::string color_name_str = color_attr.as_string();
        if (color_name_str == docraft::craft::color::kBlack.data()) {
            return docraft::DocraftColor(docraft::ColorName::Black);
        } else if (color_name_str == docraft::craft::color::kWhite.data()) {
            return docraft::DocraftColor(docraft::ColorName::White);
        } else if (color_name_str == docraft::craft::color::kRed.data()) {
            return docraft::DocraftColor(docraft::ColorName::Red);
        } else if (color_name_str == docraft::craft::color::kGreen.data()) {
            return docraft::DocraftColor(docraft::ColorName::Green);
        } else if (color_name_str == docraft::craft::color::kBlue.data()) {
            return docraft::DocraftColor(docraft::ColorName::Blue);
        } else if (color_name_str == docraft::craft::color::kYellow.data()) {
            return docraft::DocraftColor(docraft::ColorName::Yellow);
        } else if (color_name_str == docraft::craft::color::kCyan.data()) {
            return docraft::DocraftColor(docraft::ColorName::Cyan);
        } else if (color_name_str == docraft::craft::color::kMagenta.data()) {
            return docraft::DocraftColor(docraft::ColorName::Magenta);
        } else {
            //default to black
            return docraft::DocraftColor(docraft::ColorName::Black);
        }
    }

    void configure_docraft_node_attributes(const std::shared_ptr<docraft::model::DocraftNode> &node,
                                           const pugi::xml_node &craft_language_source) {
        //get attributes
        if (auto x_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kX.data())) {
        //    node->set_x(x_attr.as_float());
        }
        if (auto y_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kY.data())) {
          //  node->set_y(y_attr.as_float());
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
            if (position_str == docraft::craft::basic::attribute::position_type::kBlock.data()) {
                //node->set_position(docraft::model::DocraftPositionType::kBlock);
            } else if (position_str == docraft::craft::basic::attribute::position_type::kAbsolute.data()) {
              //  node->set_position(docraft::model::DocraftPositionType::kAbsolute);
            }
        }
        if (auto weight_attr = craft_language_source.attribute(docraft::craft::basic::attribute::kWeight.data())) {
            node->set_weight(weight_attr.as_float());
        }
    }

    void configure_section_attributes(const std::shared_ptr<docraft::model::DocraftSection> &node,
                                      const pugi::xml_node &craft_language_source) {
        //get attributes
        if (auto margin_top_attr = craft_language_source.
                attribute(docraft::craft::section::attribute::kMarginTop.data())) {
            node->set_margin_top(margin_top_attr.as_float());
        }
        if (auto margin_bottom_attr = craft_language_source.
                attribute(docraft::craft::section::attribute::kMarginBottom.data())) {
            node->set_margin_bottom(margin_bottom_attr.as_float());
        }
        if (auto margin_left_attr = craft_language_source.
                attribute(docraft::craft::section::attribute::kMarginLeft.data())) {
            node->set_margin_left(margin_left_attr.as_float());
        }
        if (auto margin_right_attr = craft_language_source.
                attribute(docraft::craft::section::attribute::kMarginRight.data())) {
            node->set_margin_right(margin_right_attr.as_float());
        }
        configure_docraft_node_attributes(node, craft_language_source);
    }
}

namespace docraft::craft::parser {
    void DocraftRectangleParser::apply_attributes_to(const std::shared_ptr<model::DocraftRectangle> &from,
                                                     const std::shared_ptr<model::DocraftRectangle> &to) {
        //apply from properties to to object
        to->set_background_color(from->background_color());
        to->set_border_color(from->border_color());
    }

    std::shared_ptr<model::DocraftNode> DocraftRectangleParser::parse(const pugi::xml_node &craft_language_source) {
        auto rectangle = std::make_shared<model::DocraftRectangle>();
        if (auto background_color_attr = craft_language_source.attribute(
            elements::rectangle::attribute::kBackgroundColor.data())) {
            rectangle->set_background_color(get_docraft_color(background_color_attr));
        }
        if (auto border_color_attr = craft_language_source.attribute(elements::rectangle::attribute::kBorderColor.data())) {
            rectangle->set_border_color(get_docraft_color(border_color_attr));
        }
        configure_docraft_node_attributes(rectangle, craft_language_source);
        return rectangle;
    }

    std::shared_ptr<model::DocraftNode> DocraftHeaderParser::parse(const pugi::xml_node &craft_language_source) {
        auto header_node = std::make_shared<model::DocraftHeader>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
            DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(header_node, rect);
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
        //get text content
        text_node->set_text(craft_language_source.child_value());
        //get attributes
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
            if (style_str == style::kBold.data()) {
                text_node->set_style(model::TextStyle::kBold);
            } else if (style_str == style::kItalic.data()) {
                text_node->set_style(model::TextStyle::kItalic);
            } else if (style_str == style::kBoldItalic.data()) {
                text_node->set_style(model::TextStyle::kBoldItalic);
            } else {
                text_node->set_style(model::TextStyle::kNormal);
            }
        }
        if (auto alignment_attr = craft_language_source.attribute(elements::text::attribute::kAlignment.data())) {
            std::string alignment_str = alignment_attr.as_string();
            if (alignment_str == alignment::kCenter.data()) {
                text_node->set_alignment(model::TextAlignment::kCenter);
            } else if (alignment_str == alignment::kRight.data()) {
                text_node->set_alignment(model::TextAlignment::kRight);
            } else if (alignment_str == alignment::kJustified.data()) {
                text_node->set_alignment(model::TextAlignment::kJustified);
            } else {
                text_node->set_alignment(model::TextAlignment::kLeft);
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
        //handle model (next)
        //check if it has TableHeader with TableTile
        if (auto table_header = craft_language_source.child(elements::kTableHeader.data())) {
            int col_number =0;
            std::vector<std::string> titles;
            for (auto title: table_header.children()) {
                if (title.name() == std::string(elements::kTableTitle.data())) {
                    col_number++;
                    table_node->add_title_node(std::make_shared<model::DocraftText>(title.child_value()));
                    titles.emplace_back(title.child_value());
                } else {
                    throw std::invalid_argument(std::string(title.name()) + " cannot be placed in a table header");
                }
            }
            table_node->set_titles(titles);
            table_node->set_cols(col_number);
            if (auto table_body = craft_language_source.child(elements::kTableBody.data())) {
                for (auto row: table_body.children()) {
                    if (row.name() == std::string(elements::kTableRow.data())) {
                        if (!row.children().empty()) {
                            const std::vector<std::string> support_type = {elements::kText.data(), elements::kImage.data()};
                            for (auto col: row.children()) {
                                bool supported = false;
                                for (const auto &item: support_type) {
                                    if (item == col.name()) {
                                        supported = true;
                                        break;
                                    }
                                }
                                if (supported) {
                                    if (col.name() == elements::kText.data()) {
                                        DocraftTextParser text_parser;
                                        auto text_node = text_parser.parse(col);
                                        table_node->add_content_node(text_node);
                                    } else if (col.name() == elements::kImage.data()) {
                                        DocraftImageParser image_parser;
                                        auto image = image_parser.parse(col);
                                        table_node->add_content_node(image);
                                    }
                                } else {
                                    throw std::runtime_error(
                                        std::string(col.name()) + " is not supported in the table body");
                                }
                            }
                        }
                    } else {
                        throw std::invalid_argument(std::string(row.name()) + " cannot be placed in a table body");
                    }
                }
            }
        } else {
            throw std::invalid_argument(std::string(elements::kTableHeader.data()) + " tag not found, it is mandatory");
        }
        configure_docraft_node_attributes(table_node, craft_language_source);
        return table_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftLayoutParser::parse(const pugi::xml_node &craft_language_source) {
        auto layout_node = std::make_shared<model::DocraftLayout>();
        if (auto orientation = craft_language_source.attribute(elements::layout::attribute::kOrientation.data())) {
            if (orientation.as_string() == orientation::kHorizontal.data()) {
                layout_node->set_orientation(model::LayoutOrientation::kHorizontal);
            } else {
                layout_node->set_orientation(model::LayoutOrientation::kVertical);
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
} // docraft

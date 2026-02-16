#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_page_number.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftPageNumberParser::parse(const pugi::xml_node &craft_language_source) {
        auto page_number_node = std::make_shared<model::DocraftPageNumber>();

        if (auto font_size_attr = craft_language_source.attribute(elements::text::attribute::kFontSize.data())) {
            page_number_node->set_font_size(font_size_attr.as_float());
        }
        if (auto font_name_attr = craft_language_source.attribute(elements::text::attribute::kFontName.data())) {
            page_number_node->set_font_name(font_name_attr.as_string());
        }
        if (auto color_attr = craft_language_source.attribute(basic::attribute::kColor.data())) {
            page_number_node->set_color(detail::get_docraft_color(color_attr));
        }
        if (auto style_attr = craft_language_source.attribute(elements::text::attribute::kStyle.data())) {
            std::string style_str = style_attr.as_string();
            if (style_str == std::string{style::kBold}) {
                page_number_node->set_style(model::TextStyle::kBold);
            } else if (style_str == std::string{style::kItalic}) {
                page_number_node->set_style(model::TextStyle::kItalic);
            } else if (style_str == std::string{style::kBoldItalic}) {
                page_number_node->set_style(model::TextStyle::kBoldItalic);
            } else if (style_str == std::string{style::kNormal}) {
                page_number_node->set_style(model::TextStyle::kNormal);
            } else {
                throw std::invalid_argument("Invalid text style: " + style_str);
            }
        }
        if (auto alignment_attr = craft_language_source.attribute(elements::text::attribute::kAlignment.data())) {
            std::string alignment_str = alignment_attr.as_string();
            if (alignment_str == std::string{alignment::kCenter}) {
                page_number_node->set_alignment(model::TextAlignment::kCenter);
            } else if (alignment_str == std::string{alignment::kRight}) {
                page_number_node->set_alignment(model::TextAlignment::kRight);
            } else if (alignment_str == std::string{alignment::kJustified}) {
                page_number_node->set_alignment(model::TextAlignment::kJustified);
            } else if (alignment_str == std::string{alignment::kLeft}) {
                page_number_node->set_alignment(model::TextAlignment::kLeft);
            } else {
                throw std::invalid_argument("Invalid text alignment: " + alignment_str);
            }
        }
        if (auto underline_attr = craft_language_source.attribute(elements::text::attribute::kUnderline.data())) {
            page_number_node->set_underline(underline_attr.as_bool());
        }

        detail::configure_docraft_node_attributes(page_number_node, craft_language_source);
        return page_number_node;
    }
}

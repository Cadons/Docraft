#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_text.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftTextParser::parse(const pugi::xml_node &craft_language_source) {
        auto text_node = std::make_shared<model::DocraftText>();

        text_node->set_text(craft_language_source.child_value());

        if (auto font_size_attr = craft_language_source.attribute(elements::text::attribute::kFontSize.data())) {
            text_node->set_font_size(font_size_attr.as_float());
        }
        if (auto font_name_attr = craft_language_source.attribute(elements::text::attribute::kFontName.data())) {
            text_node->set_font_name(font_name_attr.as_string());
        }
        if (auto color_attr = craft_language_source.attribute(basic::attribute::kColor.data())) {
            text_node->set_color(detail::get_docraft_color(color_attr));
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

        detail::configure_docraft_node_attributes(text_node, craft_language_source);
        return text_node;
    }
}

#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_settings.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftSettingsParser::parse(const pugi::xml_node &craft_language_source) {
        auto settings_node = std::make_shared<model::DocraftSettings>();
        for (auto font_node: craft_language_source.children()) {
            if (font_node.name() == std::string{elements::settings::kFonts.data()}) {
                for (auto font: font_node.children(elements::settings::fonts::kFont.data())) {
                    model::setting::DocraftFont docraft_font;
                    if (auto name_attr = font.attribute(elements::settings::fonts::attribute::kName.data())) {
                        docraft_font.name = name_attr.as_string();
                    } else {
                        throw std::invalid_argument(
                            std::string{elements::settings::fonts::attribute::kName.data()} +
                            " attribute is required for a font");
                    }
                    detail::add_external_fonts_from_node(font,
                                                         elements::settings::fonts::font_type::kFontNormal.data(),
                                                         docraft_font);
                    detail::add_external_fonts_from_node(font,
                                                         elements::settings::fonts::font_type::kFontBold.data(),
                                                         docraft_font);
                    detail::add_external_fonts_from_node(font,
                                                         elements::settings::fonts::font_type::kFontItalic.data(),
                                                         docraft_font);
                    detail::add_external_fonts_from_node(
                        font, elements::settings::fonts::font_type::kFontBoldItalic.data(), docraft_font);
                    settings_node->add_font(docraft_font);
                }
            } else {
                throw std::invalid_argument(std::string(font_node.name()) + " cannot be placed in settings");
            }
        }

        detail::configure_docraft_node_attributes(settings_node, craft_language_source);
        return settings_node;
    }
}

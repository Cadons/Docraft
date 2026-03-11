#include "docraft/craft/parser/docraft_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_settings.h"

namespace docraft::craft::parser {
    namespace {
        model::DocraftPageSize parse_page_size(const std::string &size_str) {
            if (size_str == "A3" || size_str == "a3") {
                return model::DocraftPageSize::kA3;
            }
            if (size_str == "A5" || size_str == "a5") {
                return model::DocraftPageSize::kA5;
            }
            if (size_str == "Letter" || size_str == "letter") {
                return model::DocraftPageSize::kLetter;
            }
            if (size_str == "Legal" || size_str == "legal") {
                return model::DocraftPageSize::kLegal;
            }
            if (size_str == "A4" || size_str == "a4") {
                return model::DocraftPageSize::kA4;
            }
            throw std::invalid_argument("Invalid page_size: " + size_str);
        }

        model::DocraftPageOrientation parse_page_orientation(const std::string &orientation_str) {
            if (orientation_str == "landscape") {
                return model::DocraftPageOrientation::kLandscape;
            }
            if (orientation_str == "portrait") {
                return model::DocraftPageOrientation::kPortrait;
            }
            throw std::invalid_argument("Invalid page_orientation: " + orientation_str);
        }

        void parse_page_format(const pugi::xml_node &craft_language_source,
                               const std::shared_ptr<model::DocraftSettings> &settings_node) {
            if (auto page_size_attr = craft_language_source.attribute(
                    elements::settings::attribute::kPageSize.data())) {
                const std::string size_str = page_size_attr.as_string();
                const auto size = parse_page_size(size_str);
                model::DocraftPageOrientation orientation = model::DocraftPageOrientation::kPortrait;
                if (auto orientation_attr = craft_language_source.attribute(
                        elements::settings::attribute::kPageOrientation.data())) {
                    orientation = parse_page_orientation(orientation_attr.as_string());
                }
                settings_node->set_page_format(size, orientation);
            } else if (auto orientation_attr = craft_language_source.attribute(
                    elements::settings::attribute::kPageOrientation.data())) {
                const auto orientation = parse_page_orientation(orientation_attr.as_string());
                settings_node->set_page_format(model::DocraftPageSize::kA4, orientation);
            }
        }

        void parse_section_ratios(const pugi::xml_node &craft_language_source,
                                  const std::shared_ptr<model::DocraftSettings> &settings_node) {
            const auto header_attr = craft_language_source.attribute(
                elements::settings::attribute::kHeaderRatio.data());
            const auto body_attr = craft_language_source.attribute(
                elements::settings::attribute::kBodyRatio.data());
            const auto footer_attr = craft_language_source.attribute(
                elements::settings::attribute::kFooterRatio.data());

            if (!header_attr && !body_attr && !footer_attr) {
                return;
            }

            const float header_ratio = header_attr ? header_attr.as_float() : 0.06F;
            const float body_ratio = body_attr ? body_attr.as_float() : 0.88F;
            const float footer_ratio = footer_attr ? footer_attr.as_float() : 0.06F;

            if (header_ratio < 0.0F || body_ratio < 0.0F || footer_ratio < 0.0F) {
                throw std::invalid_argument("Section ratios must be non-negative");
            }
            if (header_ratio + body_ratio + footer_ratio > 1.0F + 1e-6F) {
                throw std::invalid_argument("Section ratios must sum to 1.0 or less");
            }

            settings_node->set_section_ratios(header_ratio, body_ratio, footer_ratio);
        }

        void parse_fonts(const pugi::xml_node &craft_language_source,
                         const std::shared_ptr<model::DocraftSettings> &settings_node) {
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
        }
    } // namespace

    std::shared_ptr<model::DocraftNode> DocraftSettingsParser::parse(const pugi::xml_node &craft_language_source) {
        auto settings_node = std::make_shared<model::DocraftSettings>();
        parse_page_format(craft_language_source, settings_node);
        parse_section_ratios(craft_language_source, settings_node);
        parse_fonts(craft_language_source, settings_node);

        detail::configure_docraft_node_attributes(settings_node, craft_language_source);
        return settings_node;
    }
}

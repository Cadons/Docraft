#include "craft/parser/docraft_parser_helpers.h"

#include <cctype>
#include <stdexcept>

namespace docraft::craft::parser::detail {
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

    DocraftColor get_docraft_color(const pugi::xml_attribute &color_attr) {
        std::string color_name_str = color_attr.as_string();
        if (color_name_str.empty()) {
            throw std::invalid_argument("Color attribute cannot be empty");
        }

        // Support template expressions: ${data("fieldname")} or ${variable_name}
        // These will be resolved during the templating phase
        if (color_name_str.find("${") != std::string::npos && color_name_str.find("}") != std::string::npos) {
            // For template expressions, use a neutral default color that will be overridden during templating
            // We store the template expression as a special hex value and the templating engine will resolve it
            return DocraftColor(color_name_str);
        }

        if (color_name_str[0] == '#') {
            if (!is_hex_color(color_name_str)) {
                throw std::invalid_argument("Invalid hex color: " + color_name_str);
            }
            return DocraftColor(color_name_str);
        }

        if (color_name_str == std::string{color::kBlack}) {
            return DocraftColor(ColorName::kBlack);
        } else if (color_name_str == std::string{color::kWhite}) {
            return DocraftColor(ColorName::kWhite);
        } else if (color_name_str == std::string{color::kRed}) {
            return DocraftColor(ColorName::kRed);
        } else if (color_name_str == std::string{color::kGreen}) {
            return DocraftColor(ColorName::kGreen);
        } else if (color_name_str == std::string{color::kBlue}) {
            return DocraftColor(ColorName::kBlue);
        } else if (color_name_str == std::string{color::kYellow}) {
            return DocraftColor(ColorName::kYellow);
        } else if (color_name_str == std::string{color::kCyan}) {
            return DocraftColor(ColorName::kCyan);
        } else if (color_name_str == std::string{color::kMagenta}) {
            return DocraftColor(ColorName::kMagenta);
        } else if (color_name_str == std::string{color::kPurple}) {
            return DocraftColor(ColorName::kPurple);
        }

        throw std::invalid_argument("Unknown color: " + color_name_str);
    }

    void add_external_fonts_from_node(const pugi::xml_node &font_node,
                                      const char *child_name,
                                      model::setting::DocraftFont &docraft_font) {
        for (auto external_font: font_node.children(child_name)) {
            model::setting::DocraftExternalFont docraft_external_font;
            docraft_external_font.path = external_font.attribute(
                elements::settings::fonts::font_type::attribute::kSrc.data()).as_string();
            std::string node_type = external_font.name();
            const std::string font_prefix = std::string{elements::settings::fonts::kFont.data()};
            if (node_type.rfind(font_prefix, 0) == 0) {
                node_type = node_type.substr(font_prefix.size());
            }
            if (node_type.empty() || node_type == std::string{"Normal"}) {
                docraft_external_font.name = docraft_font.name;
            } else {
                docraft_external_font.name = docraft_font.name + std::string{"-"} + node_type;
            }
            docraft_font.external_fonts.push_back(docraft_external_font);
        }
    }

    void configure_docraft_node_attributes(const std::shared_ptr<model::DocraftNode> &node,
                                           const pugi::xml_node &craft_language_source) {
        bool has_position_attr = false;
        bool has_xy = false;
        if (auto name_attr = craft_language_source.attribute(basic::attribute::kNodeName.data())) {
            node->set_name(name_attr.as_string());
        }
        if (auto x_attr = craft_language_source.attribute(basic::attribute::kX.data())) {
            node->set_position({x_attr.as_float(), node->position().y});
            has_xy = true;
        }
        if (auto y_attr = craft_language_source.attribute(basic::attribute::kY.data())) {
            node->set_position({node->position().x, y_attr.as_float()});
            has_xy = true;
        }
        if (auto width_attr = craft_language_source.attribute(basic::attribute::kWidth.data())) {
            node->set_width(width_attr.as_float());
        }
        if (auto height_attr = craft_language_source.attribute(basic::attribute::kHeight.data())) {
            node->set_height(height_attr.as_float());
        }
        if (auto auto_fill_height_attr = craft_language_source.attribute(
                basic::attribute::kAutoFillHeight.data())) {
            node->set_auto_fill_height(auto_fill_height_attr.as_bool());
        }
        if (auto auto_fill_width_attr = craft_language_source.attribute(
                basic::attribute::kAutoFillWidth.data())) {
            node->set_auto_fill_width(auto_fill_width_attr.as_bool());
        }
        if (auto padding_attr = craft_language_source.attribute(basic::attribute::kPadding.data())) {
            node->set_padding(padding_attr.as_float());
        }
        if (auto position_attr = craft_language_source.attribute(basic::attribute::kPosition.data())) {
            std::string position_str = position_attr.as_string();
            has_position_attr = true;
            if (position_str == std::string{basic::attribute::position_type::kBlock}) {
                node->set_position_mode(model::DocraftPositionType::kBlock);
            } else if (position_str == std::string{basic::attribute::position_type::kAbsolute}) {
                node->set_position_mode(model::DocraftPositionType::kAbsolute);
            } else {
                throw std::invalid_argument("Invalid position value: " + position_str);
            }
        }
        if (has_xy && !has_position_attr) {
            node->set_position_mode(model::DocraftPositionType::kAbsolute);
        }
        if (auto weight_attr = craft_language_source.attribute(basic::attribute::kWeight.data())) {
            node->set_weight(weight_attr.as_float());
        }
        if (auto z_index_attr = craft_language_source.attribute(basic::attribute::kZIndex.data())) {
            node->set_z_index(z_index_attr.as_int());
        }
        if (auto visible_attr = craft_language_source.attribute(basic::attribute::kVisible.data())) {
            node->set_visible(visible_attr.as_bool());
        }
    }

    void configure_section_attributes(const std::shared_ptr<model::DocraftSection> &node,
                                      const pugi::xml_node &craft_language_source) {
        if (auto margin_top_attr = craft_language_source.attribute(section::attribute::kMarginTop.data())) {
            node->set_margin_top(margin_top_attr.as_float());
        }
        if (auto margin_bottom_attr = craft_language_source.attribute(section::attribute::kMarginBottom.data())) {
            node->set_margin_bottom(margin_bottom_attr.as_float());
        }
        if (auto margin_left_attr = craft_language_source.attribute(section::attribute::kMarginLeft.data())) {
            node->set_margin_left(margin_left_attr.as_float());
        }
        if (auto margin_right_attr = craft_language_source.attribute(section::attribute::kMarginRight.data())) {
            node->set_margin_right(margin_right_attr.as_float());
        }
        configure_docraft_node_attributes(node, craft_language_source);
    }
}

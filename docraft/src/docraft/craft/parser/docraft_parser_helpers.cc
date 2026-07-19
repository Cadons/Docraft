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

#include "docraft/craft/parser/docraft_parser_helpers.h"

#include <cctype>

#include "docraft/exception/docraft_exceptions.h"

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

    std::string get_color_attribute_raw(const pugi::xml_attribute &color_attr) {
        std::string color_name_str = color_attr.as_string();
        if (color_name_str.empty()) {
            throw docraft::exception::InvalidInputException("Color attribute cannot be empty");
        }
        return color_name_str;
    }

    DocraftColor parse_docraft_color(const std::string &color_name_str) {
        if (color_name_str[0] == '#') {
            if (!is_hex_color(color_name_str)) {
                throw docraft::exception::InvalidInputException("Invalid hex color: " + color_name_str);
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

        throw docraft::exception::InvalidInputException("Unknown color: " + color_name_str);
    }

    DocraftCommonAttributes parse_common_node_attributes(const pugi::xml_node& craft_language_source)
    {
        DocraftCommonAttributes common;
        bool has_position_attr = false;
        bool has_xy = false;

        if (auto name_attr = craft_language_source.attribute(basic::attribute::kNodeName.data()))
        {
            common.name = name_attr.as_string();
        }
        if (auto x_attr = craft_language_source.attribute(basic::attribute::kX.data()))
        {
            common.x = x_attr.as_float();
            has_xy = true;
        }
        if (auto y_attr = craft_language_source.attribute(basic::attribute::kY.data()))
        {
            common.y = y_attr.as_float();
            has_xy = true;
        }
        if (auto width_attr = craft_language_source.attribute(basic::attribute::kWidth.data()))
        {
            common.width = width_attr.as_float();
        }
        if (auto height_attr = craft_language_source.attribute(basic::attribute::kHeight.data()))
        {
            common.height = height_attr.as_float();
        }
        if (auto padding_attr = craft_language_source.attribute(basic::attribute::kPadding.data()))
        {
            common.padding = padding_attr.as_float();
        }
        if (auto margin_attr = craft_language_source.attribute(basic::attribute::kMargin.data()))
        {
            common.margin = margin_attr.as_float();
        }
        if (auto margin_top_attr = craft_language_source.attribute(basic::attribute::kMarginTop.data()))
        {
            common.margin_top = margin_top_attr.as_float();
        }
        if (auto margin_right_attr = craft_language_source.attribute(basic::attribute::kMarginRight.data()))
        {
            common.margin_right = margin_right_attr.as_float();
        }
        if (auto margin_bottom_attr = craft_language_source.attribute(basic::attribute::kMarginBottom.data()))
        {
            common.margin_bottom = margin_bottom_attr.as_float();
        }
        if (auto margin_left_attr = craft_language_source.attribute(basic::attribute::kMarginLeft.data()))
        {
            common.margin_left = margin_left_attr.as_float();
        }
        if (auto position_attr = craft_language_source.attribute(basic::attribute::kPosition.data()))
        {
            const std::string position_str = position_attr.as_string();
            has_position_attr = true;
            if (position_str == std::string{basic::attribute::position_type::kBlock})
            {
                common.position_mode = PositionMode::kBlock;
            } else if (position_str == std::string{basic::attribute::position_type::kAbsolute})
            {
                common.position_mode = PositionMode::kAbsolute;
            } else {
                throw docraft::exception::InvalidInputException("Invalid position value: " + position_str);
            }
        }
        if (has_xy && !has_position_attr)
        {
            // Explicit coordinates with no explicit "position" attribute imply absolute
            // placement, mirroring the legacy parser's behavior.
            common.position_mode = PositionMode::kAbsolute;
        }
        if (auto weight_attr = craft_language_source.attribute(basic::attribute::kWeight.data())) {
            common.weight = weight_attr.as_float();
        }
        if (auto z_index_attr = craft_language_source.attribute(basic::attribute::kZIndex.data())) {
            common.z_index = z_index_attr.as_int();
        }
        if (auto visible_attr = craft_language_source.attribute(basic::attribute::kVisible.data()))
        {
            common.visible = visible_attr.as_bool();
        }

        return common;
    }
} // namespace docraft::craft::parser::detail

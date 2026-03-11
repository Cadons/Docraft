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

#include "docraft/docraft_color.h"

#include <cctype>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "docraft/utils/docraft_parser_utilis.h"

namespace docraft {
    void DocraftColor::convert_known_color(ColorName name) {
        switch (color_name_) {
            case ColorName::kBlack: rgb_ = RGB(0.0F, 0.0F, 0.0F, 1.0F);
                break;
            case ColorName::kWhite: rgb_ = RGB(1.0F, 1.0F, 1.0F, 1.0F);
                break;
            case ColorName::kRed: rgb_ = RGB(1.0F, 0.0F, 0.0F, 1.0F);
                break;
            case ColorName::kGreen: rgb_ = RGB(0.0F, 1.0F, 0.0F, 1.0F);
                break;
            case ColorName::kBlue: rgb_ = RGB(0.0F, 0.0F, 1.0F, 1.0F);
                break;
            case ColorName::kYellow: rgb_ = RGB(1.0F, 1.0F, 0.0F, 1.0F);
                break;
            case ColorName::kCyan: rgb_ = RGB(0.0F, 1.0F, 1.0F, 1.0F);
                break;
            case ColorName::kMagenta: rgb_ = RGB(1.0F, 0.0F, 1.0F, 1.0F);
                break;
            case ColorName::kPurple: rgb_ = RGB(0.5F, 0.0F, 0.5F, 1.0F);
                break;
        }
    }

    DocraftColor::DocraftColor(ColorName name) : color_name_(name) {
        convert_known_color(name);
    }

    DocraftColor::DocraftColor(float r, float g, float b, float a) : color_name_(), rgb_(RGB(r, g, b, a)) {
    }

    DocraftColor::DocraftColor(const std::string &hex_code) {
        // Support template expressions: ${data("fieldname")} or ${variable_name}
        if (docraft::utils::DocraftParserUtilis::is_data_request(hex_code)|| docraft::utils::DocraftParserUtilis::is_template_variable(hex_code)){
            // Store as template expression and use default black color
            template_expression_ = hex_code;
            color_name_ = ColorName::kBlack;
            convert_known_color(ColorName::kBlack);
            return;
        }

        if ((hex_code.size() != 7 && hex_code.size() != 9) || hex_code[0] != '#') {
            throw std::invalid_argument("Invalid hex code: " + hex_code);
        }
        for (size_t i = 1; i < hex_code.size(); ++i) {
            if (!std::isxdigit(static_cast<unsigned char>(hex_code[i]))) {
                throw std::invalid_argument("Invalid hex code: " + hex_code);
            }
        }
        //example of color: #RRGGBB or #RRGGBBAA
        unsigned int r = std::stoul(std::string(hex_code.substr(1, 2)), nullptr, 16);
        unsigned int g = std::stoul(std::string(hex_code.substr(3, 2)), nullptr, 16);
        unsigned int b = std::stoul(std::string(hex_code.substr(5, 2)), nullptr, 16);
        if (hex_code.size() == 7) {
            rgb_ = RGB(r / 255.0F, g / 255.0F, b / 255.0F, 1.0F);
            return;
        }
        //handle alpha
        unsigned int a = std::stoul(std::string(hex_code.substr(7, 2)), nullptr, 16);
        rgb_ = RGB(r / 255.0F, g / 255.0F, b / 255.0F, a / 255.0F);
    }

    RGB DocraftColor::toRGB() const {
        return rgb_;
    }

    ColorName DocraftColor::toColorName() const {
        return color_name_;
    }

    DocraftColor DocraftColor::fromColorName(ColorName name) {
        return DocraftColor(name);
    }

    DocraftColor DocraftColor::fromRGB(float r, float g, float b, float a) {
        return DocraftColor(r, g, b, a);
    }

    bool DocraftColor::is_template_expression() const {
        return !template_expression_.empty();
    }

    const std::string& DocraftColor::template_expression() const {
        return template_expression_;
    }
} // docraft

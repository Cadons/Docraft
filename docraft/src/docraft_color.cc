#include "docraft_color.h"

#include <cctype>
#include <iostream>
#include <ostream>
#include <stdexcept>

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
} // docraft

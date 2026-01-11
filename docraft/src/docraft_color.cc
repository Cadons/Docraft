#include "docraft_color.h"

#include <iostream>
#include <ostream>

namespace docraft {
    void DocraftColor::convert_known_color(ColorName name) {
        switch (color_name_) {
            case ColorName::Black: rgb_ = RGB(0.0F, 0.0F, 0.0F, 1.0F);
                break;
            case ColorName::White: rgb_ = RGB(1.0F, 1.0F, 1.0F, 1.0F);
                break;
            case ColorName::Red: rgb_ = RGB(1.0F, 0.0F, 0.0F, 1.0F);
                break;
            case ColorName::Green: rgb_ = RGB(0.0F, 1.0F, 0.0F, 1.0F);
                break;
            case ColorName::Blue: rgb_ = RGB(0.0F, 0.0F, 1.0F, 1.0F);
                break;
            case ColorName::Yellow: rgb_ = RGB(1.0F, 1.0F, 0.0F, 1.0F);
                break;
            case ColorName::Cyan: rgb_ = RGB(0.0F, 1.0F, 1.0F, 1.0F);
                break;
            case ColorName::Magenta: rgb_ = RGB(1.0F, 0.0F, 1.0F, 1.0F);
                break;
            case ColorName::Purple: rgb_ = RGB(0.5F, 0.0F, 0.5F, 1.0F);
                break;
        }
    }

    DocraftColor::DocraftColor(ColorName name) : color_name_(name) {
        convert_known_color(name);
    }

    DocraftColor::DocraftColor(float r, float g, float b, float a) : rgb_(RGB(r, g, b, a)) {
    }

    DocraftColor::DocraftColor(const std::string &hex_code) {
        //convert hex to rgb
        if (hex_code.size() != 7 || hex_code[0] != '#') {
            if (hex_code.size() != 9) {
                std::cerr << "Invalid hex code: " << hex_code << std::endl;
                rgb_ = RGB(0.0F, 0.0F, 0.0F, 1.0F); //default black
                return;
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

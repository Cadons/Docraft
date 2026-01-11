#pragma once
#include <string>

namespace docraft {
    enum class ColorName {
        Black,
        White,
        Red,
        Green,
        Blue,
        Yellow,
        Magenta,
        Cyan,
        Purple
    };

    struct RGB {
        float r=0.0F;
        float g=0.0F;
        float b=0.0F;
        float a=1.0F;
        std::string to_hex() const {
            auto clamp_byte = [](int v) -> int { return v < 0 ? 0 : (v > 255 ? 255 : v); };
            int r_int = clamp_byte(static_cast<int>(r * 255.0F + 0.5F));//convert float to int between 0-255
            int g_int = clamp_byte(static_cast<int>(g * 255.0F + 0.5F));
            int b_int = clamp_byte(static_cast<int>(b * 255.0F + 0.5F));
            int a_int = clamp_byte(static_cast<int>(a * 255.0F + 0.5F));

            char buf[10]; // "#RRGGBBAA" + null
            std::snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X", r_int, g_int, b_int, a_int);//print the string in the buffer
            return std::string(buf);
        }
    };

    class DocraftColor {
    public:

        explicit DocraftColor(ColorName name = ColorName::Black);

        explicit DocraftColor(float r, float g, float b, float a = 1.0F);

        explicit DocraftColor(const std::string &hex_code);

        ~DocraftColor() = default;

        [[nodiscard]] RGB toRGB() const;

        [[nodiscard]] ColorName toColorName() const;

        static DocraftColor fromColorName(ColorName name);

        static DocraftColor fromRGB(float r, float g, float b, float a = 1.0F);

    private:
        ColorName color_name_;
        RGB rgb_;
        void convert_known_color(ColorName name);

    };
} // docraft

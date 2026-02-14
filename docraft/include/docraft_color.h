#pragma once
#include <string>

namespace docraft {
    /**
     * @brief Named colors supported by the library.
     */
    enum class ColorName {
        kBlack,
        kWhite,
        kRed,
        kGreen,
        kBlue,
        kYellow,
        kMagenta,
        kCyan,
        kPurple
    };

    /**
     * @brief RGBA color with components in the [0,1] range.
     */
    struct RGB {
        float r=0.0F;
        float g=0.0F;
        float b=0.0F;
        float a=1.0F;
        /**
         * @brief Converts the RGBA color to a hex string #RRGGBBAA.
         * @return Hex string in #RRGGBBAA format.
         */
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

    /**
     * @brief Color helper that supports named, RGB, and hex representations.
     */
    class DocraftColor {
    public:

        /**
         * @brief Creates a color from a known name.
         * @param name Named color.
         */
        explicit DocraftColor(ColorName name = ColorName::kBlack);

        /**
         * @brief Creates a color from RGBA components.
         * @param r Red in [0,1].
         * @param g Green in [0,1].
         * @param b Blue in [0,1].
         * @param a Alpha in [0,1].
         */
        explicit DocraftColor(float r, float g, float b, float a = 1.0F);

        /**
         * @brief Creates a color from a hex code (#RRGGBB or #RRGGBBAA).
         * @param hex_code Hex string.
         */
        explicit DocraftColor(const std::string &hex_code);

        /**
         * @brief Destructor.
         */
        ~DocraftColor() = default;

        /**
         * @brief Returns the RGBA representation.
         * @return RGBA components.
         */
        [[nodiscard]] RGB toRGB() const;

        /**
         * @brief Returns the named color (if available).
         * @return ColorName enum value.
         */
        [[nodiscard]] ColorName toColorName() const;

        /**
         * @brief Creates a DocraftColor from a named color.
         * @param name Named color.
         * @return DocraftColor instance.
         */
        static DocraftColor fromColorName(ColorName name);

        /**
         * @brief Creates a DocraftColor from RGBA components.
         * @param r Red in [0,1].
         * @param g Green in [0,1].
         * @param b Blue in [0,1].
         * @param a Alpha in [0,1].
         * @return DocraftColor instance.
         */
        static DocraftColor fromRGB(float r, float g, float b, float a = 1.0F);

    private:
        ColorName color_name_;
        RGB rgb_;
        void convert_known_color(ColorName name);

    };
} // docraft

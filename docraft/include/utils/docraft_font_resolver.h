#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "model/docraft_text.h"

namespace docraft::utils {
    /**
     * @brief Resolves a font family + style into an available font name.
     *
     * The resolver builds a per-family index of available variants (regular/bold/italic/bold-italic)
     * based on a list of built-in fonts and fonts registered in DocraftFontRegistry.
     * It returns the closest available variant when an exact match is missing.
     */
    class DocraftFontResolver {
    public:
        /**
         * @brief Creates a font resolver with an empty index.
         */
        DocraftFontResolver() = default;
        /**
         * @brief Rebuilds the internal index from the provided font name lists.
         * @param builtin_fonts Built-in font names.
         * @param registered_fonts Fonts registered at runtime.
         */
        void rebuild_index(const std::vector<std::string> &builtin_fonts,
                           const std::vector<std::string> &registered_fonts);
        /**
         * @brief Resolves the best matching font for a given request.
         * @param requested Requested font family name.
         * @param style Requested text style.
         * @return Resolved font name (may be empty).
         */
        std::string resolve(const std::string &requested,
                            docraft::model::TextStyle style) const;
    private:
        /**
         * @brief Per-family font variant mapping.
         */
        struct FontVariants {
            std::string regular;
            std::string bold;
            std::string italic;
            std::string bold_italic;
        };
        /**
         * @brief Parsed font name with style metadata.
         */
        struct ParsedFont {
            std::string family;
            bool bold = false;
            bool italic = false;
        };
        /**
         * @brief Parses a font name into family and style flags.
         * @param name Font name.
         * @return Parsed font info.
         */
        static ParsedFont parse_font_name(const std::string &name);
        /**
         * @brief Lowercases a string.
         * @param value Input string.
         * @return Lowercased string.
         */
        static std::string to_lower(std::string value);
        /**
         * @brief Trims trailing separators from a string.
         * @param value Input string.
         * @return Trimmed string.
         */
        static std::string trim_trailing_separators(std::string value);
        std::unordered_map<std::string, FontVariants> index_;
    };
} // docraft::utils

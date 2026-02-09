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
     */
    class DocraftFontResolver {
    public:
        DocraftFontResolver() = default;
        /**
         * @brief Rebuilds the internal index from the provided font name lists.
         */
        void rebuild_index(const std::vector<std::string> &builtin_fonts,
                           const std::vector<std::string> &registered_fonts);
        /**
         * @brief Resolves the best matching font for a given request.
         */
        std::string resolve(const std::string &requested,
                            docraft::model::TextStyle style) const;
    private:
        struct FontVariants {
            std::string regular;
            std::string bold;
            std::string italic;
            std::string bold_italic;
        };
        struct ParsedFont {
            std::string family;
            bool bold = false;
            bool italic = false;
        };
        static ParsedFont parse_font_name(const std::string &name);
        static std::string to_lower(std::string value);
        static std::string trim_trailing_separators(std::string value);
        std::unordered_map<std::string, FontVariants> index_;
    };
} // docraft::utils

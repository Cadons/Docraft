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

#pragma once

#include "docraft/docraft_lib.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace docraft::utils {
    /**
     * @brief Text styles used when resolving a font variant.
     */
    enum class TextStyle
    {
        kNormal,
        kBold,
        kItalic,
        kBoldItalic
    };

    /**
     * @brief Resolves a font family + style into an available font name.
     *
     * The resolver builds a per-family index of available variants (regular/bold/italic/bold-italic)
     * based on a list of built-in fonts and fonts registered in DocraftFontRegistry.
     * It returns the closest available variant when an exact match is missing.
     */
    class DOCRAFT_LIB DocraftFontResolver {
    public:
        /**
         * @brief Creates a font resolver with an empty index.
         */
        DocraftFontResolver() = default;
        /**
         * @brief Returns libharu's 14 built-in base font names (Courier/Helvetica/Times
         * variants, Symbol, ZapfDingbats) -- these resolve by name with no registration
         * needed, unlike a custom TTF registered via DocraftFontRegistry.
         */
        static std::vector<std::string> builtin_font_names();
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
                            TextStyle style) const;

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

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

#include <pugixml.hpp>

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/docraft_craft_parsed_element.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/docraft_color.h"

namespace docraft::craft::parser::detail {
    /**
     * @brief Checks whether a string is a valid hex color.
     * Supports hex colors (\#RRGGBB or \#RRGGBBAA).
     * @param color String to check.
     * @return True if the string is a valid hex color, false otherwise.
     */
    bool is_hex_color(const std::string &color);

    /**
     * @brief Extracts a color attribute's raw text, unresolved. The value may be a hex
     * code, a named color, or a `${...}` template expression -- interpretation is
     * deferred to `parse_docraft_color`, called after templating has resolved any
     * expression (see `DocraftLoomTreeBuilder`), since this parser stage runs before
     * templating and has no template engine to resolve against.
     * @param color_attr XML attribute containing the color value.
     * @return The attribute's raw text.
     * @throws docraft::exception::InvalidInputException if the attribute value is empty.
     */
    std::string get_color_attribute_raw(const pugi::xml_attribute &color_attr);

    /**
     * @brief Parses an already-resolved (template-free) color string into a DocraftColor.
     * Supports hex colors (e.g., \#RRGGBB or \#RRGGBBAA) and named colors (e.g., "red",
     * "blue").
     * @param color_str The resolved color string.
     * @return A DocraftColor object representing the parsed color.
     * @throws docraft::exception::InvalidInputException if the color string is not in a valid format or is an unsupported named color.
     */
    DocraftColor parse_docraft_color(const std::string &color_str);

    /**
     * @brief Parses the universal attributes (name/x/y/width/height/padding/weight/
     * z_index/visible/position) shared by every Craft-language tag into a
     * `DocraftCommonAttributes`. Generic and tag-agnostic -- called once per element by
     * `DocraftCraftLanguageParser`, not by individual `IDocraftParser` implementations.
     * @param craft_language_source The XML node to read attributes from.
     * @return The parsed common attributes.
     */
    DocraftCommonAttributes parse_common_node_attributes(const pugi::xml_node& craft_language_source);

    /**
     * @brief Parses a `style` attribute value (e.g. "bold") into a `ParsedTextStyle`.
     * Shared by every text-like parser (`Text`/`Title`/`Subtitle`, `PageNumber`,
     * `Paragraph`).
     * @param style_str The raw attribute string.
     * @return The parsed style.
     * @throws docraft::exception::InvalidInputException if the string is not a recognized style.
     */
    ParsedTextStyle parse_text_style(const std::string& style_str);

    /**
     * @brief Parses an `alignment` attribute value (e.g. "center") into a
     * `ParsedTextAlignment`. Shared by every text-like parser (`Text`/`Title`/
     * `Subtitle`, `PageNumber`, `Paragraph`).
     * @param alignment_str The raw attribute string.
     * @return The parsed alignment.
     * @throws docraft::exception::InvalidInputException if the string is not a recognized alignment.
     */
    ParsedTextAlignment parse_text_alignment(const std::string& alignment_str);

    /**
     * @brief Trims leading/trailing whitespace (space, tab, newline, CR, form feed,
     * vertical tab) from a string.
     * @param text The string to trim.
     * @return The trimmed string.
     */
    std::string trim_whitespace(const std::string& text);
} // namespace docraft::craft::parser::detail

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
     * @brief Parses a color attribute value into a DocraftColor object.
     * Supports hex colors (e.g., \#RRGGBB or \#RRGGBBAA), named colors (e.g., "red",
     * "blue") and template expressions (e.g. "${variable}"), which `DocraftColor` itself
     * stores verbatim for a later templating pass to resolve.
     * @param color_attr XML attribute containing the color value.
     * @return A DocraftColor object representing the parsed color.
     * @throws docraft::exception::InvalidInputException if the color string is not in a valid format or is an unsupported named color.
     */
    DocraftColor get_docraft_color(const pugi::xml_attribute &color_attr);

    /**
     * @brief Parses the universal attributes (name/x/y/width/height/padding/weight/
     * z_index/visible/position) shared by every Craft-language tag into a
     * `DocraftCommonAttributes`. Generic and tag-agnostic -- called once per element by
     * `DocraftCraftLanguageParser`, not by individual `IDocraftParser` implementations.
     * @param craft_language_source The XML node to read attributes from.
     * @return The parsed common attributes.
     */
    DocraftCommonAttributes parse_common_node_attributes(const pugi::xml_node& craft_language_source);
} // namespace docraft::craft::parser::detail

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

#include <any>

#include "docraft/docraft_lib.h"
#include <pugixml.hpp>

namespace docraft::craft {
    /**
     * @brief Interface for Craft language tag parsers.
     *
     * @details Implementations translate a single XML element into a tag-specific, plain
     * data-transfer struct (e.g. `ParsedRectangleData`), returned type-erased via
     * `std::any`. This interface has no dependency on any rendering/layout engine --
     * common attributes (x/y/width/height/...) and child recursion are handled once,
     * generically, by `DocraftCraftLanguageParser` itself, not by individual parsers.
     */
    class DOCRAFT_LIB IDocraftParser
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IDocraftParser() = default;

        /**
         * @brief Parses an XML element into its tag-specific payload.
         * @param craft_language_source XML node to parse.
         * @return Type-erased `Parsed<Tag>Data` struct.
         */
        virtual std::any parse(const pugi::xml_node& craft_language_source) = 0;
    };
} // namespace docraft::craft

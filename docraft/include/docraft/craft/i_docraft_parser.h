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
#include <pugixml.hpp>

#include "docraft/model/docraft_node.h"

namespace docraft::craft {
        /**
         * @brief Interface for Craft language node parsers.
         *
         * Implementations translate a single XML node into a Docraft model node.
         */
        class DOCRAFT_LIB IDocraftParser {
        public:
                /**
                 * @brief Virtual destructor.
                 */
                virtual ~IDocraftParser() = default;
                /**
                 * @brief Parses an XML node into a Docraft node.
                 * @param craft_language_source XML node to parse.
                 * @return Parsed Docraft node.
                 */
                virtual std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) = 0;
        };
} // docraft

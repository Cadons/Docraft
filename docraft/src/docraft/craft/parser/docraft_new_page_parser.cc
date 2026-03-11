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

#include "docraft/craft/parser/docraft_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_new_page.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftNewPageParser::parse(const pugi::xml_node &craft_language_source) {
        auto page_break = std::make_shared<model::DocraftNewPage>();
        detail::configure_docraft_node_attributes(page_break, craft_language_source);
        return page_break;
    }
} // namespace docraft::craft::parser

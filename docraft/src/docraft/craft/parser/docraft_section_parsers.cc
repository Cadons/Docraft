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
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_footer.h"
#include "docraft/model/docraft_header.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftHeaderParser::parse(const pugi::xml_node &craft_language_source) {
        auto header_node = std::make_shared<model::DocraftHeader>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, header_node);
        detail::configure_section_attributes(header_node, craft_language_source);
        header_node->set_position_mode(model::DocraftPositionType::kBlock);
        return header_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftBodyParser::parse(const pugi::xml_node &craft_language_source) {
        auto body_node = std::make_shared<model::DocraftBody>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, body_node);
        detail::configure_section_attributes(body_node, craft_language_source);
        body_node->set_position_mode(model::DocraftPositionType::kBlock);
        return body_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftFooterParser::parse(const pugi::xml_node &craft_language_source) {
        auto footer_node = std::make_shared<model::DocraftFooter>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, footer_node);
        detail::configure_section_attributes(footer_node, craft_language_source);
        footer_node->set_position_mode(model::DocraftPositionType::kBlock);
        return footer_node;
    }
}

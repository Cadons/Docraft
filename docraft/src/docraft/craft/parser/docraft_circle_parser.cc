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

#include "docraft/craft/parser/docraft_circle_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_circle.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftCircleParser::parse(const pugi::xml_node &craft_language_source) {
        auto circle = std::make_shared<model::DocraftCircle>();
        if (auto background_color_attr = craft_language_source.attribute(
                elements::circle::attribute::kBackgroundColor.data())) {
            circle->set_background_color(detail::get_docraft_color(background_color_attr));
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::circle::attribute::kBorderColor.data())) {
            circle->set_border_color(detail::get_docraft_color(border_color_attr));
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::circle::attribute::kBorderWidth.data())) {
            circle->set_border_width(border_width_attr.as_float());
        }
        detail::configure_docraft_node_attributes(circle, craft_language_source);
        return circle;
    }
} // namespace docraft::craft::parser

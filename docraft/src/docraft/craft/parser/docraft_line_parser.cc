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

#include "docraft/craft/parser/docraft_line_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_line.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftLineParser::parse(const pugi::xml_node &craft_language_source) {
        auto line = std::make_shared<model::DocraftLine>();
        if (auto x1_attr = craft_language_source.attribute(elements::line::attribute::kX1.data())) {
            line->set_start({.x = x1_attr.as_float(), .y = line->start().y});
        }
        if (auto y1_attr = craft_language_source.attribute(elements::line::attribute::kY1.data())) {
            line->set_start({.x = line->start().x, .y = y1_attr.as_float()});
        }
        if (auto x2_attr = craft_language_source.attribute(elements::line::attribute::kX2.data())) {
            line->set_end({.x = x2_attr.as_float(), .y = line->end().y});
        }
        if (auto y2_attr = craft_language_source.attribute(elements::line::attribute::kY2.data())) {
            line->set_end({.x = line->end().x, .y = y2_attr.as_float()});
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::line::attribute::kBorderColor.data())) {
            line->set_border_color(detail::get_docraft_color(border_color_attr));
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::line::attribute::kBorderWidth.data())) {
            line->set_border_width(border_width_attr.as_float());
        }
        detail::configure_docraft_node_attributes(line, craft_language_source);
        return line;
    }
} // namespace docraft::craft::parser

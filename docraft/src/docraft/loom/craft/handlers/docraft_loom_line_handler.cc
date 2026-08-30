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

#include "docraft/loom/craft/handlers/docraft_loom_line_handler.h"

#include <any>

#include "docraft/craft/parser/docraft_line_parser.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_line.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomLineHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedLineData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomLine>();
        nodes::Position start = node->start();
        nodes::Position end = node->end();
        if (data.x1)
        {
            start.x = *data.x1;
        }
        if (data.y1)
        {
            start.y = *data.y1;
        }
        if (data.x2)
        {
            end.x = *data.x2;
        }
        if (data.y2)
        {
            end.y = *data.y2;
        }
        node->set_start(start);
        node->set_end(end);
        if (data.border_color.has_value())
        {
            node->set_border_color(context.resolve_color(*data.border_color));
        }
        if (data.border_width.has_value())
        {
            node->set_border_width(*data.border_width);
        }
        apply_common_attributes(*node, element.common);
        return node;
    }
} // namespace docraft::loom::craft

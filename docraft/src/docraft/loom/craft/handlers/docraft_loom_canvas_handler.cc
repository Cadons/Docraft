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

#include "docraft/loom/craft/handlers/docraft_loom_canvas_handler.h"

#include <any>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomCanvasHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        // Canvas has no auto-sizing (its children are free-positioned, not stacked), so
        // it needs an explicit bound to clip against -- unlike Rectangle, an unset
        // width/height is a configuration error, not a "shrink to content" default.
        if (!element.common.width.has_value() || !element.common.height.has_value())
        {
            throw docraft::exception::InvalidInputException(
                "<Canvas> requires explicit 'width' and 'height' attributes");
        }
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedRectangleData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomCanvas>();
        apply_shape_style(*node, data, [&context](const std::string& c) { return context.resolve_color(c); });
        apply_common_attributes(*node, element.common);
        context.add_children(node, element.children);
        return node;
    }
} // namespace docraft::loom::craft

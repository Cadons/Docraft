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

#include "docraft/loom/craft/handlers/docraft_loom_curve_line_handler.h"

#include <any>

#include "docraft/craft/parser/docraft_curve_line_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_curve_line.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomCurveLineHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedCurveLineData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomCurveLine>();
        // An open curve is well defined through 2 points, where a closed <Polygon> needs
        // 3 -- rejecting fewer here, rather than letting the renderer skip the node,
        // keeps a mis-specified curve from silently drawing nothing.
        if (data.points.size() < 2U)
        {
            throw docraft::exception::InvalidInputException(
                "<CurveLine> requires at least 2 points, got " + std::to_string(data.points.size()));
        }
        node->set_points(data.points);
        if (data.border_color.has_value())
        {
            node->set_border_color(context.resolve_color(*data.border_color));
        }
        if (data.border_width.has_value())
        {
            node->set_border_width(*data.border_width);
        }
        if (data.border_style.has_value())
        {
            node->set_border_style(to_loom_line_style(*data.border_style));
        }
        apply_common_attributes(*node, element.common);
        return node;
    }
} // namespace docraft::loom::craft

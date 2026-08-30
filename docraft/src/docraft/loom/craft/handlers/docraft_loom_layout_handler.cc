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

#include "docraft/loom/craft/handlers/docraft_loom_layout_handler.h"

#include <algorithm>
#include <any>
#include <vector>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomLayoutHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedLayoutData&>(element.data);

        // The only way to specify per-child weights is `weight="..."` on each child of
        // <Layout> itself (already validated at parse time -- `weight` is rejected
        // anywhere else). A missing weight defaults to 1.0, matching
        // distribute_weighted_amounts()'s own default for a missing/non-positive entry.
        std::vector<float> weights;
        const bool any_child_weight = std::ranges::any_of(
            element.children,
            [](const auto& child) { return child->common.weight.has_value(); });
        if (any_child_weight)
        {
            weights.reserve(element.children.size());
            for (const auto& child : element.children)
            {
                weights.push_back(child->common.weight.value_or(1.0F));
            }
        }

        if (data.orientation == docraft::craft::parser::ParsedLayoutOrientation::kHorizontal)
        {
            auto node = std::make_shared<nodes::DocraftLoomHStack>();
            if (data.spacing)
            {
                node->set_spacing(*data.spacing);
            }
            if (!weights.empty())
            {
                node->set_weights(weights);
            }
            apply_common_attributes(*node, element.common);
            context.add_children(node, element.children);
            return node;
        }
        auto node = std::make_shared<nodes::DocraftLoomVStack>();
        if (data.spacing)
        {
            node->set_spacing(*data.spacing);
        }
        if (!weights.empty())
        {
            node->set_weights(weights);
        }
        apply_common_attributes(*node, element.common);
        context.add_children(node, element.children);
        return node;
    }
} // namespace docraft::loom::craft

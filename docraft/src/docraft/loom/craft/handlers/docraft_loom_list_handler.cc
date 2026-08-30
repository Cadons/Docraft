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

#include "docraft/loom/craft/handlers/docraft_loom_list_handler.h"

#include <any>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_list.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomListHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedListData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomList>();
        node->set_kind(data.kind == docraft::craft::parser::ParsedListKind::kOrdered
                           ? nodes::ListKind::kOrdered
                           : nodes::ListKind::kUnordered);
        if (data.ordered_style.has_value())
        {
            node->set_ordered_style(*data.ordered_style == docraft::craft::parser::ParsedOrderedListStyle::kNumber
                                        ? nodes::OrderedListStyle::kNumber
                                        : nodes::OrderedListStyle::kRoman);
        }
        if (data.unordered_dot.has_value())
        {
            nodes::UnorderedListDot dot = nodes::UnorderedListDot::kCircle;
            switch (*data.unordered_dot)
            {
            case docraft::craft::parser::ParsedUnorderedListDot::kDash:
                dot = nodes::UnorderedListDot::kDash;
                break;
            case docraft::craft::parser::ParsedUnorderedListDot::kStar:
                dot = nodes::UnorderedListDot::kStar;
                break;
            case docraft::craft::parser::ParsedUnorderedListDot::kBox:
                dot = nodes::UnorderedListDot::kBox;
                break;
            case docraft::craft::parser::ParsedUnorderedListDot::kCircle:
            default:
                dot = nodes::UnorderedListDot::kCircle;
                break;
            }
            node->set_unordered_dot(dot);
        }
        apply_common_attributes(*node, element.common);
        context.add_children(node, element.children);
        return node;
    }
} // namespace docraft::loom::craft

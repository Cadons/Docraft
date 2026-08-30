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

#include "docraft/loom/craft/handlers/docraft_loom_text_handler.h"

#include <any>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_subtitle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_title.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomTextHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedTextData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomText>();
        context.fill_text_node(*node, data);
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomTitleHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedTextData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomTitle>();
        // fill_text_node only overwrites fields data actually set (see its own
        // if (data.xxx) checks) -- an explicit font-size attribute still wins,
        // otherwise DocraftLoomTitle's own constructor default stands.
        context.fill_text_node(*node, data);
        // Keep margin coherent with whatever font_size ended up being used (constructor
        // default or an explicit attribute) -- 1em, matching the constructor's own
        // convention -- so overriding font-size doesn't leave a disproportionate,
        // fixed-constant gap. An explicit margin attribute below still overrides this.
        node->set_margin(node->font_size());
        apply_common_attributes(*node, element.common);
        return node;
    }

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomSubtitleHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedTextData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomSubtitle>();
        context.fill_text_node(*node, data);
        // See DocraftLoomTitleHandler::build's identical comment above.
        node->set_margin(node->font_size());
        apply_common_attributes(*node, element.common);
        return node;
    }
} // namespace docraft::loom::craft

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

#include "docraft/loom/craft/handlers/docraft_loom_paragraph_handler.h"

#include <any>

#include "docraft/craft/parser/docraft_paragraph_parser.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomParagraphHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedParagraphData&>(element.data);
        auto node = std::make_shared<nodes::DocraftLoomParagraph>();
        if (data.line_spacing.has_value())
        {
            node->set_line_spacing(*data.line_spacing);
        }
        if (data.space_before.has_value())
        {
            node->set_space_before(*data.space_before);
        }
        if (data.space_after.has_value())
        {
            node->set_space_after(*data.space_after);
        }
        if (data.alignment.has_value())
        {
            node->set_alignment(to_loom_alignment(*data.alignment));
        }
        apply_common_attributes(*node, element.common);
        // Bare PCDATA directly inside <Paragraph>text</Paragraph> is invisible to
        // add_children() below (it only walks XML element children, see ParsedElement's
        // recursion) -- surface it here as an implicit Text child so a Paragraph with no
        // explicit <Text> wrapper still renders, matching the natural way authors write
        // Craft Language paragraphs.
        if (!data.text.empty())
        {
            auto text_node = std::make_shared<nodes::DocraftLoomText>();
            text_node->set_text(context.render_template_text(data.text));
            node->add_child(text_node);
        }
        context.add_children(node, element.children);
        return node;
    }
} // namespace docraft::loom::craft

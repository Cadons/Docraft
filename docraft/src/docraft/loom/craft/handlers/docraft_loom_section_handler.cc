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

#include "docraft/loom/craft/handlers/docraft_loom_section_handler.h"

#include <any>

#include "docraft/craft/parser/docraft_section_parsers.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::craft {
    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomSectionHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedSectionData&>(element.data);
        // Header/Body/Footer only ever use background_color/border_color/border_width
        // from ParsedSectionData -- never Rectangle's width_/height_/padding_ -- so a
        // VStack (with its own composed DocraftLoomShapeStyle, see
        // DocraftLoomVStack::style()) is a closer fit than Rectangle and reuses the
        // vertical-stacking flow instead of duplicating it.
        auto node = std::make_shared<nodes::DocraftLoomVStack>();
        apply_shape_style(*node, data, [&context](const std::string& c) { return context.resolve_color(c); });
        // Margins (margin_top/bottom/left/right) are not a rectangle concept -- they are
        // consumed directly from ParsedSectionData by DocraftLoomCraftLanguageParser to
        // build a DocraftLoomPdfCreator::Margins, not applied here.
        apply_common_attributes(*node, element.common);
        context.add_children(node, element.children);
        return node;
    }
} // namespace docraft::loom::craft

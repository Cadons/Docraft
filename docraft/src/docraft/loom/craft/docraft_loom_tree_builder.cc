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

#include "docraft/loom/craft/docraft_loom_tree_builder.h"

#include <any>

#include <nlohmann/json.hpp>

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_foreach_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/craft/docraft_loom_tag_handler_registry.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"

namespace docraft::loom::craft {
    DocraftLoomTreeBuilder::DocraftLoomTreeBuilder(
        std::shared_ptr<docraft::templating::DocraftTemplateEngine> template_engine)
        : template_engine_(template_engine
                               ? std::move(template_engine)
                               : std::make_shared<docraft::templating::DocraftTemplateEngine>())
    {
        charts::register_builtin_chart_styles();
        register_builtin_tag_handlers();
    }

    void DocraftLoomTreeBuilder::set_default_font_family(const std::string& font_family)
    {
        default_font_family_ = font_family;
    }

    namespace tokens = docraft::craft;
    namespace parser = docraft::craft::parser;

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomTreeBuilder::build(
        const std::shared_ptr<docraft::craft::DocraftParsedElement>& element)
    {
        if (!element)
        {
            return nullptr;
        }
        // "visible" is resolved here, not by the (engine-agnostic) craft parser: an
        // explicitly-invisible element's subtree simply isn't constructed.
        if (element->common.visible.has_value() && !*element->common.visible)
        {
            return nullptr;
        }

        const std::string& tag = element->tag_name;

        // Every tag dispatches to a per-tag IDocraftLoomTagHandler registered in
        // register_builtin_tag_handlers() -- adding a new tag means adding one handler
        // class plus one registration line there, not a new branch/method here.
        if (auto* handler = DocraftLoomTagHandlerRegistry::instance().find(tag))
        {
            return handler->build(*element, *this);
        }

        throw docraft::exception::DataFormatException("DocraftLoomTreeBuilder: unrecognized tag '" + tag + "'");
    }

    void DocraftLoomTreeBuilder::add_children(const std::shared_ptr<nodes::DocraftLoomNode>& parent,
                                              const std::vector<std::shared_ptr<ParsedElement>>& children)
    {
        for (const auto& child : children)
        {
            if (child->tag_name == std::string{tokens::elements::templating::kForeach})
            {
                expand_foreach(parent, *child);
                continue;
            }
            if (auto built = build(child))
            {
                parent->add_child(built);
            }
        }
    }

    void DocraftLoomTreeBuilder::expand_foreach(const std::shared_ptr<nodes::DocraftLoomNode>& parent,
                                                const ParsedElement& foreach_element)
    {
        const auto& data = std::any_cast<const parser::ParsedForeachData&>(foreach_element.data);

        if (data.model)
        {
            // `model` may itself be (or contain) a `${variable}`/`${data("field")}`
            // expression -- e.g. model="${employees}", or, nested inside an outer
            // Foreach, model="${data("subitems")}" -- so it must be resolved against the
            // *outer* current_foreach_item_ (still in scope at this point) before the
            // single-quote normalization and JSON parsing below.
            const std::string resolved_model = render_template_text(*data.model);
            const std::string normalized_model = normalize_single_quoted_json(resolved_model);

            nlohmann::json items;
            try
            {
                items = nlohmann::json::parse(normalized_model);
            }
            catch (const nlohmann::json::exception& e)
            {
                throw docraft::exception::DataFormatException(
                    std::string{"<Foreach> 'model' is not valid JSON: "} + e.what());
            }
            if (!items.is_array())
            {
                throw docraft::exception::DataFormatException("<Foreach> 'model' must resolve to a JSON array");
            }

            const nlohmann::json* previous_item = current_foreach_item_;
            for (const auto& item : items)
            {
                current_foreach_item_ = &item;
                add_children(parent, foreach_element.children);
            }
            current_foreach_item_ = previous_item;
            return;
        }

        const int count = data.count.value_or(0);
        for (int i = 0; i < count; ++i)
        {
            add_children(parent, foreach_element.children);
        }
    }

    DocraftColor DocraftLoomTreeBuilder::resolve_color(const std::string& raw) const
    {
        return parser::detail::parse_docraft_color(render_template_text(raw));
    }

    std::string DocraftLoomTreeBuilder::render_template_text(const std::string& text) const
    {
        return current_foreach_item_ != nullptr
                   ? template_engine_->render_template_string_foreach_item(text, *current_foreach_item_)
                   : template_engine_->render_template_string(text);
    }

    void DocraftLoomTreeBuilder::fill_text_node(nodes::DocraftLoomText& node, const parser::ParsedTextData& data) const
    {
        node.set_text(render_template_text(data.text));
        if (data.font_size)
        {
            node.set_font_size(*data.font_size);
        }
        if (data.font_name)
        {
            node.set_font_family(*data.font_name);
        }
        else if (default_font_family_)
        {
            node.set_font_family(*default_font_family_);
        }
        if (data.color)
        {
            node.set_color(resolve_color(*data.color));
        }
        if (data.style)
        {
            apply_style(node, *data.style);
        }
        if (data.alignment)
        {
            node.set_alignment(to_loom_alignment(*data.alignment));
        }
        if (data.underline)
        {
            node.set_underline(*data.underline);
        }
        if (data.strikeout)
        {
            node.set_strikeout(*data.strikeout);
        }
    }

    void DocraftLoomTreeBuilder::fill_page_number_node(nodes::DocraftLoomPageNumber& node,
                                                       const parser::ParsedPageNumberData& data) const
    {
        if (data.font_size)
        {
            node.set_font_size(*data.font_size);
        }
        if (data.font_name)
        {
            node.set_font_family(*data.font_name);
        }
        else if (default_font_family_)
        {
            node.set_font_family(*default_font_family_);
        }
        if (data.color)
        {
            node.set_color(resolve_color(*data.color));
        }
        if (data.style)
        {
            apply_style(node, *data.style);
        }
        if (data.alignment)
        {
            node.set_alignment(to_loom_alignment(*data.alignment));
        }
        if (data.underline)
        {
            node.set_underline(*data.underline);
        }
        if (data.strikeout)
        {
            node.set_strikeout(*data.strikeout);
        }
        if (data.format)
        {
            node.set_format(*data.format);
        }
    }

    void DocraftLoomTreeBuilder::fill_image_node(nodes::DocraftLoomImage& node,
                                                 const parser::ParsedImageData& data) const
    {
        if (data.raw_data && data.raw_pixel_width && data.raw_pixel_height)
        {
            node.set_raw_data(*data.raw_data, *data.raw_pixel_width, *data.raw_pixel_height);
        }
        else if (data.path)
        {
            node.set_path(render_template_text(*data.path));
        }
    }
} // namespace docraft::loom::craft

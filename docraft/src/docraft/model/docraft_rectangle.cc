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

#include "docraft/model/docraft_rectangle.h"

#include <iostream>
#include <ostream>

#include "docraft/model/docraft_clone_utils.h"
#include "docraft/renderer/docraft_renderer.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::model {
    void DocraftRectangle::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        LOG_DEBUG("Drawing rectangle");
        context->renderer()->render_rectangle(*this);
        draw_children(context);
    }

    void DocraftRectangle::set_background_color(const DocraftColor &color) {
        background_color_ = color;
    }

    void DocraftRectangle::set_border_color(const DocraftColor &color) {
        border_color_ = color;
    }

    void DocraftRectangle::set_border_width(float width) {
        border_width_ = width;
    }

    const DocraftColor &DocraftRectangle::background_color() const {
        return background_color_;
    }

    const DocraftColor &DocraftRectangle::border_color() const {
        return border_color_;
    }

    float DocraftRectangle::border_width() const {
        return border_width_;
    }

    void DocraftRectangle::set_background_color_template_expression(const std::string &expression) {
        background_color_expression_ = expression;
    }

    void DocraftRectangle::set_border_color_template_expression(const std::string &expression) {
        border_color_expression_ = expression;
    }

    const std::string &DocraftRectangle::background_color_template_expression() const {
        return background_color_expression_;
    }

    const std::string &DocraftRectangle::border_color_template_expression() const {
        return border_color_expression_;
    }

    std::shared_ptr<DocraftNode> DocraftRectangle::clone() const {
        auto copy = std::make_shared<DocraftRectangle>(*this);
        copy->clear_children();
        for (const auto &child: children()) {
            copy->add_child(clone_node(child));
        }
        return copy;
    }
} // docraft

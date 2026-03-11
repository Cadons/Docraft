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

#include "docraft/model/docraft_section.h"

#include "docraft/model/docraft_clone_utils.h"
#include "docraft/renderer/docraft_renderer.h"

namespace docraft::model {
    DocraftSection::DocraftSection() {
        set_border_width(0.0F);
        set_padding(2.0F);
    }

    DocraftSection::DocraftSection(DocraftSection *node) : DocraftRectangle(node),
                                                           margins_(node->margins_),
                                                           margin_top_(node->margin_top_),
                                                           margin_bottom_(node->margin_bottom_),
                                                           margin_left_(node->margin_left_),
                                                           margin_right_(node->margin_right_) {
    }

    void DocraftSection::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        context->renderer()->render_section(*this);
        draw_children(context);
    }
    float DocraftSection::margins() const {
        return margins_;
    }
    float DocraftSection::margin_top() const {
        return margin_top_;
    }
    float DocraftSection::margin_bottom() const {
        return margin_bottom_;
    }
    float DocraftSection::margin_left() const {
        return margin_left_;
    }
    float DocraftSection::margin_right() const {
        return margin_right_;
    }
    void DocraftSection::set_margins(float margins) {
        margins_ = margins;
    }
    void DocraftSection::set_margin_top(float margin_top) {
        margin_top_ = margin_top;
    }
    void DocraftSection::set_margin_bottom(float margin_bottom) {
        margin_bottom_ = margin_bottom;
    }
    void DocraftSection::set_margin_left(float margin_left) {
        margin_left_ = margin_left;
    }
    void DocraftSection::set_margin_right(float margin_right) {
        margin_right_ = margin_right;
    }

    std::shared_ptr<DocraftNode> DocraftSection::clone() const {
        auto copy = std::make_shared<DocraftSection>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        return copy;
    }
} // Docraft

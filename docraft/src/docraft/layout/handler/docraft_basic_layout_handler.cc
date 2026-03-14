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

#include "docraft/layout/handler/docraft_basic_layout_handler.h"


#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_section.h"

namespace docraft::layout::handler {
    void DocraftBasicLayoutHandler::compute(const std::shared_ptr<model::DocraftNode> &node,
                                            model::DocraftTransform *box,
                                            DocraftCursor& cursor) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }

        if (node->position_mode()==model::DocraftPositionType::kBlock) {
            box->set_position({.x=cursor.x(), .y=cursor.y()});
        }else {
            box->set_position({.x=node->position().x, .y=node->position().y});
        }

        const bool is_rectangle = static_cast<bool>(std::dynamic_pointer_cast<model::DocraftRectangle>(node));
        const float child_width = box->width();
        const float child_height = box->height();

        if (node->position_mode() == model::DocraftPositionType::kBlock) {
            if (node->width() > 0.0F) {
                box->set_width(node->width());
            } else if (node->auto_fill_width()) {
                box->set_width(std::max(context()->available_space(), child_width));
            } else if (is_rectangle) {
                box->set_width(child_width);
            } else if (context()->available_space() < node->width() || node->width() == 0.0F) {
                box->set_width(context()->available_space());
            } else {
                box->set_width(node->width());
            }
        } else {
            box->set_width(node->width());
        }

        if (node->height() > 0.0F) {
            box->set_height(node->height());
        } else if (is_rectangle) {
            // Check if this is a rectangle with children (not a Section, which handles its own padding)
            auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(node);
            const bool is_section = static_cast<bool>(std::dynamic_pointer_cast<model::DocraftSection>(node));
            if (rect && !rect->children().empty() && !is_section) {
                // Add default bottom padding when rectangle has children
                const float k_rectangle_bottom_padding = rect->padding();
                box->set_height(child_height + k_rectangle_bottom_padding);
            } else {
                box->set_height(child_height);
            }
        } else {
            box->set_height(node->height());
        }
    }

    bool DocraftBasicLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                           model::DocraftTransform *result,
                                           DocraftCursor& cursor) {
        compute(request, result, cursor);
        return true;
    }
} // docraft

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

#include "docraft/layout/handler/docraft_layout_blank_line.h"

namespace docraft::layout::handler {
    void DocraftLayoutBlankLine::compute(const std::shared_ptr<model::DocraftBlankLine> &node,
                                         model::DocraftTransform* box,
                                         DocraftCursor& cursor) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }
        node->set_weight(1.0F); //blank line takes full width
        box->set_width(context()->available_space());//get full available width
        if (node->height() > 0.0F) {
            box->set_height(node->height());
        } else {
            box->set_height(1.0F); //default height for blank line
        }
        box->set_position({.x=cursor.x(), .y=cursor.y()});

    }

    bool DocraftLayoutBlankLine::handle(const std::shared_ptr<model::DocraftNode> &request,
                                        model::DocraftTransform * result,
                                        DocraftCursor& cursor) {
        if (auto blank_line_node = std::dynamic_pointer_cast<model::DocraftBlankLine>(request)) {
            compute(blank_line_node, result, cursor);
            return true;
        }
        return false;
    }
} // docraft

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

#include "docraft/layout/handler/docraft_layout_handler.h"

#include "docraft/layout/docraft_layout_engine.h"
#define PADDING 20.0F

namespace docraft::layout::handler {
    void DocraftLayoutHandler::compute(const std::shared_ptr<model::DocraftLayout> &node,
                                       model::DocraftTransform *box,
                                       DocraftCursor& cursor) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }
        // If the layout has a weight, the parent already scoped available_space to that share.
        if (node->weight()!=-1.0F) {
            node->set_width(context()->available_space());
            box->set_width(node->width());
        }
        cursor.pop_direction(); //remove layout direction
    }

    bool DocraftLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                      model::DocraftTransform *result,
                                      DocraftCursor& cursor) {
        if (auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(request)) {
            compute(layout_node, result, cursor);
            return true;
        }
        return false;
    }
} // docraft

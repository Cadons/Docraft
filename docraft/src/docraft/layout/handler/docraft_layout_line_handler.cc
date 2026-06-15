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

#include "docraft/layout/handler/docraft_layout_line_handler.h"

namespace docraft::layout::handler {
    void DocraftLayoutLineHandler::compute(const std::shared_ptr<model::DocraftLine> &node,
                                           model::DocraftTransform *box,
                                           DocraftCursor &cursor) {
        if (!box) {
            throw docraft::exception::InvalidInputException("result transform is null");
        }

        // In block mode with explicit width: treat as horizontal line from cursor.
        // Otherwise: use explicit start/end points (absolute positioning).
        if (node->position_mode() == model::DocraftPositionType::kBlock && node->width() > 0.0F) {
            box->set_position({.x = cursor.x(), .y = cursor.y()});
            box->set_width(node->width());
            // Provide minimal height so the box occupies vertical space in flow.
            box->set_height(node->border_width() + 4.0F);
        }
    }

    bool DocraftLayoutLineHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                          model::DocraftTransform *result,
                                          DocraftCursor &cursor) {
        auto line_node = std::dynamic_pointer_cast<model::DocraftLine>(request);
        if (!line_node) {
            return false;
        }

        compute(line_node, result, cursor);
        // Note: cursor advancement is handled by finalize_layout in the layout engine,
        // which uses the box height we set in compute().
        return true;
    }
} // namespace docraft::layout::handler





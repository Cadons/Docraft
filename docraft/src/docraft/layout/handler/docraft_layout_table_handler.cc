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

#include "docraft/layout/handler/docraft_layout_table_handler.h"

#include <fmt/format.h>

#include "docraft/exception/docraft_exceptions.h"
#include "docraft/layout/handler/docraft_layout_horizontal_table_handler.h"
#include "docraft/layout/handler/docraft_layout_vertical_table_handler.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::layout::handler {

    void DocraftLayoutTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                            model::DocraftTransform *box,
                                            DocraftCursor &cursor) {
        if (!node) {
            throw docraft::exception::InvalidInputException("table node is null");
        }

        if (node->titles().empty()) {
            if (node->position_mode() == model::DocraftPositionType::kBlock) {
                node->set_position({.x = cursor.x(), .y = cursor.y()});
            } else {
                node->set_position({.x = node->position().x, .y = node->position().y});
            }
            node->set_width(0.0F);
            node->set_height(0.0F);

            if (box) {
                box->set_position(node->position());
                box->set_width(node->width());
                box->set_height(node->height());
            }
            return;
        }

        switch (node->orientation()) {
            case model::LayoutOrientation::kHorizontal: {
                DocraftLayoutHorizontalTableHandler h_handler(edit_context());
                h_handler.compute(node, box, cursor);
                break;
            }
            case model::LayoutOrientation::kVertical: {
                DocraftLayoutVerticalTableHandler v_handler(edit_context());
                v_handler.compute(node, box, cursor);
                break;
            }
            default:
                throw docraft::exception::LayoutConfigurationException("unsupported table orientation");
        }
    }

    bool DocraftLayoutTableHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                           model::DocraftTransform *result,
                                           DocraftCursor &cursor) {
        if (auto table_node = std::dynamic_pointer_cast<model::DocraftTable>(request)) {
            compute(table_node, result, cursor);
            return true;
        }
        return false;
    }
} // namespace docraft::layout::handler

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

#pragma once

#include "docraft/docraft_lib.h"
#include "abstract_docraft_layout_handler.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_node.h"

namespace docraft::layout::handler {
    /**
     * @brief Fallback layout handler for generic nodes.
     *
     * Used when no specialized handler claims the node.
     */
    class DOCRAFT_LIB DocraftBasicLayoutHandler : public AbstractDocraftLayoutHandler<model::DocraftNode> {
    public:
        using AbstractDocraftLayoutHandler::AbstractDocraftLayoutHandler;

        /**
         * @brief Computes the layout box for a basic node.
         * @param node Node to layout.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftNode> &node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        /**
         * @brief Handles a node if no specialized handler applies.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft

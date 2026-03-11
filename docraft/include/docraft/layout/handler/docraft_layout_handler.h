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
#include "docraft/generic/chain_of_responsibility_handler.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_node.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for DocraftLayout nodes.
     *
     * Computes child boxes based on orientation and weights.
     */
    class DOCRAFT_LIB DocraftLayoutHandler :public AbstractDocraftLayoutHandler<model::DocraftLayout> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftLayout>::AbstractDocraftLayoutHandler;
        /**
         * @brief Computes the layout for a layout node.
         * @param node Layout node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftLayout> &node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        /**
         * @brief Handles a node if it is a DocraftLayout.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft

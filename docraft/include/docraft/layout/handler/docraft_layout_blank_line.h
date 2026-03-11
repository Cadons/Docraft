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
#include "docraft/model/docraft_blank_line.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for blank line nodes.
     *
     * Advances the cursor to create vertical spacing.
     */
    class DOCRAFT_LIB DocraftLayoutBlankLine :public AbstractDocraftLayoutHandler<model::DocraftBlankLine> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftBlankLine>::AbstractDocraftLayoutHandler;
        /**
         * @brief Computes the layout box for a blank line.
         * @param node Blank line node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftBlankLine> &node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        /**
         * @brief Handles a node if it is a DocraftBlankLine.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft

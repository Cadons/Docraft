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
#include "docraft/layout/handler/docraft_layout_table_handler.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for vertical tables.
     *
     * Handles DocraftTable nodes whose orientation is kVertical.
     * Row labels occupy the first column; value columns follow to the right.
     * An optional header row (htitle_nodes) sits above all data rows.
     *
     * Inherits common helpers and context access from DocraftLayoutTableHandler.
     */
    class DOCRAFT_LIB DocraftLayoutVerticalTableHandler : public DocraftLayoutTableHandler {
    public:
        using DocraftLayoutTableHandler::DocraftLayoutTableHandler;

        /**
         * @brief Computes the layout for a vertical table node.
         * @param node  Vertical table node.
         * @param box   Output transform (position + size).
         * @param cursor Layout cursor; not advanced (table manages its own cursor internally).
         */
        void compute(const std::shared_ptr<model::DocraftTable> &node,
                     model::DocraftTransform *box,
                     DocraftCursor &cursor) override;
    };
} // namespace docraft::layout::handler


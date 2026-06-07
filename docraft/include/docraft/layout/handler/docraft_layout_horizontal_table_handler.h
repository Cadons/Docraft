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

#include <optional>
#include <utility>
#include <vector>

#include "docraft/docraft_document_context.h"
#include "docraft/docraft_lib.h"
#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/layout/handler/docraft_layout_table_handler.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for horizontal tables.
     *
     * Handles DocraftTable nodes whose orientation is kHorizontal.
     * Columns are placed left-to-right; a header row precedes the data rows.
     *
     * Inherits common helpers and context access from DocraftLayoutTableHandler.
     */
    class DOCRAFT_LIB DocraftLayoutHorizontalTableHandler : public DocraftLayoutTableHandler {
    public:
        using DocraftLayoutTableHandler::DocraftLayoutTableHandler;

        /**
         * @brief Computes the layout for a horizontal table node.
         * @param node  Horizontal table node.
         * @param box   Output transform (position + size).
         * @param cursor Layout cursor; not advanced (table manages its own cursor internally).
         */
        void compute(const std::shared_ptr<model::DocraftTable> &node,
                     model::DocraftTransform *box,
                     DocraftCursor &cursor) override;

    private:
        struct TableContent {
            std::vector<std::shared_ptr<model::DocraftNode> > title_nodes;
            std::vector<std::vector<std::shared_ptr<model::DocraftNode> > > rows;
        };

        struct WidthPlan {
            float table_width = 0.0F;
            std::vector<float> col_widths;

            WidthPlan() = default;

            WidthPlan(float width, std::vector<float> widths)
                : table_width(width), col_widths(std::move(widths)) {
            }
        };

        /** Initialize per-compute state and cursor anchoring. */
        void setup_compute_state(const std::shared_ptr<model::DocraftTable> &node, DocraftCursor &cursor);

        /** Collect normalized title/content node vectors used by all compute phases. */
        static TableContent collect_table_content(const std::shared_ptr<model::DocraftTable> &node);

        /** Compute final column widths and natural title-row height. */
        WidthPlan compute_width_plan(const std::shared_ptr<model::DocraftTable> &node,
                                     const TableContent &content);

        /** Apply resolved width plan to class state consumed by row layout helpers. */
        void apply_width_plan(const WidthPlan &plan);

        /** Layout table title row and all body rows, returning body total height. */
        float layout_body_rows(const TableContent &content, float start_y, float min_row_height);

        /** Write final node/box geometry and restore layout service width state. */
        void finalize_output(const std::shared_ptr<model::DocraftTable> &node,
                             model::DocraftTransform *box,
                             float table_width,
                             float table_height);

        /** Emit debug logs for computed cell geometry. */
        static void log_cells(const std::shared_ptr<model::DocraftTable> &node);

        /** Release temporary compute resources kept in class state. */
        void clear_compute_state();

        float layout_row(const std::vector<std::shared_ptr<model::DocraftNode> > &row_nodes,
                         float row_top_y,
                         float min_row_height);

        std::shared_ptr<DocraftDocumentContext> context_;
        std::optional<DocraftLayoutEngine> engine_;
        DocraftCursor table_cursor_;
        std::size_t cols_ = 0;
        std::vector<float> col_widths_;
        std::vector<float> col_lefts_;
        float offset_x_ = 0.0F;
        float offset_y_ = 0.0F;
        float cell_padding_x_ = 0.0F;
        float cell_padding_y_ = 0.0F;
        float baseline_offset_ = 0.0F;
        float fixed_x_ = 0.0F;
        float fixed_y_ = 0.0F;
        float saved_available_space_ = 0.0F;
    };
} // namespace docraft::layout::handler


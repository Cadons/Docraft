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
#include <vector>

#include "docraft/docraft_document_context.h"
#include "docraft/docraft_lib.h"
#include "docraft/layout/docraft_layout_engine.h"
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

    private:
        struct TableData {
            std::vector<std::shared_ptr<model::DocraftNode> > title_nodes;
            std::vector<std::shared_ptr<model::DocraftNode> > flat_values;
            std::size_t row_count = 0;
            std::size_t value_cols = 0;
        };

        struct ColumnPlan {
            float title_col_width = 0.0F;
            float value_col_width = 0.0F;
            std::size_t value_cols = 0;
        };

        struct BandView {
            const std::vector<std::shared_ptr<model::DocraftNode> > *nodes = nullptr;
            const std::vector<float> *lefts = nullptr;
            const std::vector<float> *widths = nullptr;
        };

        /** Initialize per-compute state and cursor anchoring. */
        void setup_compute_state(const std::shared_ptr<model::DocraftTable> &node, DocraftCursor &cursor);

        /** Collect normalized title/value node vectors used by layout phases. */
        static TableData collect_table_data(const std::shared_ptr<model::DocraftTable> &node);

        /** Compute natural width required by the title column. */
        float compute_title_natural_width(const TableData &data);

        /** Resolve title/value column widths using weights and explicit value widths. */
        ColumnPlan resolve_column_plan(const std::shared_ptr<model::DocraftTable> &node,
                                       const TableData &data,
                                       float title_natural_width) const;

        /** Layout optional header row and return consumed height. */
        float layout_header_row(const std::shared_ptr<model::DocraftTable> &node,
                                const ColumnPlan &plan,
                                float row_top_y);

        /** Layout all table body rows and return consumed height. */
        float layout_body_rows(const TableData &data,
                               const ColumnPlan &plan,
                               float row_top_y);

        /** Write final node/box geometry and restore layout service width state. */
        void finalize_output(const std::shared_ptr<model::DocraftTable> &node,
                             model::DocraftTransform *box,
                             float table_width,
                             float table_height);

        /** Emit debug logs for computed cell geometry. */
        static void log_cells(const std::shared_ptr<model::DocraftTable> &node);

        /** Release temporary compute resources kept in class state. */
        void clear_compute_state();

        float compute_band_height(const BandView &band, float row_top_y);

        void place_band(const BandView &band, float row_top_y, float row_height) const;

        std::shared_ptr<DocraftDocumentContext> context_;
        std::optional<DocraftLayoutEngine> engine_;
        DocraftCursor table_cursor_;
        float padding_x_ = 0.0F;
        float padding_y_ = 0.0F;
        float cell_padding_x_ = 0.0F;
        float cell_padding_y_ = 0.0F;
        float baseline_offset_ = 0.0F;
        float fixed_x_ = 0.0F;
        float fixed_y_ = 0.0F;
        float saved_available_space_ = 0.0F;
    };
} // namespace docraft::layout::handler


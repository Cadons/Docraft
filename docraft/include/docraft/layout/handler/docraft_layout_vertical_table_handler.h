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

    protected:
        void setup_pipeline_state(const std::shared_ptr<model::DocraftTable> &node,
                                  DocraftCursor &cursor) override;

        void prepare_table_layout(const std::shared_ptr<model::DocraftTable> &node) override;

        [[nodiscard]] float layout_table_content(const std::shared_ptr<model::DocraftTable> &node) override;

        [[nodiscard]] float resolve_table_width() const override;

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
                                float row_top_y,
                                float min_row_height);

        /** Layout all table body rows and return consumed height. */
        float layout_body_rows(const TableData &data,
                               const ColumnPlan &plan,
                               float row_top_y,
                               float min_row_height);

        /** Build a row band for the optional horizontal titles in a vertical table. */
        [[nodiscard]] RowBand build_header_band(const std::shared_ptr<model::DocraftTable> &node,
                                                const ColumnPlan &plan) const;

        /** Build a row band for one vertical table body row (title + value cells). */
        [[nodiscard]] RowBand build_body_band(const TableData &data, const ColumnPlan &plan, std::size_t row_index) const;

        TableData data_;
        ColumnPlan plan_;
        float table_width_ = 0.0F;
    };
} // namespace docraft::layout::handler


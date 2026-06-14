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

#include "docraft/docraft_lib.h"
#include "abstract_docraft_layout_handler.h"
#include "docraft/model/docraft_table.h"
#include "docraft/docraft_document_context.h"
#include "docraft/layout/docraft_layout_engine.h"

namespace docraft::layout::handler {
    /**
     * @brief Base layout handler for table nodes.
     *
     * Handles the degenerate case of tables with no titles (zero-size output).
     * Provides shared state and helper methods for orientation-specific sub-handlers:
     *   - DocraftLayoutHorizontalTableHandler  (kHorizontal)
     *   - DocraftLayoutVerticalTableHandler    (kVertical)
     *
     * Register all three in the handler chain so each handles its own case.
     */
    class DOCRAFT_LIB DocraftLayoutTableHandler : public AbstractDocraftLayoutHandler<model::DocraftTable> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftTable>::AbstractDocraftLayoutHandler;

        /**
         * @brief Computes the layout box for a table node.
         * @param node Table node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftTable> &node, model::DocraftTransform *box,
                     DocraftCursor &cursor) override;

        /**
         * @brief Handles a node if it is a DocraftTable.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result,
                    DocraftCursor &cursor) override;

    protected:
        /**
         * @brief Template-method entrypoint used by orientation handlers.
         *
         * The parent executes the full lifecycle while children override hook methods
         * for orientation-specific logic.
         */
        void compute_with_pipeline(const std::shared_ptr<model::DocraftTable> &node,
                                   model::DocraftTransform *box,
                                   DocraftCursor &cursor);

        /** Initialize per-orientation state before table layout. */
        virtual void setup_pipeline_state(const std::shared_ptr<model::DocraftTable> &node,
                                          DocraftCursor &cursor);

        /** Resolve and cache orientation-specific geometry state. */
        virtual void prepare_table_layout(const std::shared_ptr<model::DocraftTable> &node);

        /** Layout rows/cells and return total table height. */
        [[nodiscard]] virtual float layout_table_content(const std::shared_ptr<model::DocraftTable> &node);

        /** Return computed table width after preparation/layout. */
        [[nodiscard]] virtual float resolve_table_width() const;

        static constexpr float kHorizontalCellPaddingY = 2.5F;
        static constexpr float kHorizontalCellPaddingX = 2.5F;
        static constexpr float kVerticalCellPaddingY = 2.0F;
        static constexpr float kVerticalCellPaddingX = 2.0F;

        /**
         * @brief Logical row view used by both horizontal and vertical table handlers.
         *
         * A band is one render row: same top-Y, multiple cells with independent x/width.
         * The base handler computes content sizes first, derives a single row height, then
         * places every cell with that height so row borders always align.
         */
        struct RowBand {
            std::vector<std::shared_ptr<model::DocraftNode> > nodes; // Nodes in this row, in left-to-right order.
            std::vector<float> lefts;
            //lefts represent the left edge of each cell, used for horizontal tables; ignored for vertical tables
            std::vector<float> widths;
            //widths represent the width of each cell, used for horizontal tables; ignored for vertical tables
            float min_row_height = 0.0F; // Minimum height for this row band (from table.padding()).
        };

        /**
         * @brief Initializes common state for cell computation.
         */
        void initialize_base_state(const std::shared_ptr<model::DocraftTable> &node,
                                   DocraftCursor &cursor,
                                   float cell_padding_x,
                                   float cell_padding_y);

        /**
         * @brief Ensures title nodes exist; creates synthetic text nodes if needed.
         */
        static void ensure_title_nodes(const std::shared_ptr<model::DocraftTable> &node);

        /**
         * @brief Flattens 2D content grid into a single vector.
         */
        static std::vector<std::shared_ptr<model::DocraftNode> > flatten_content_nodes(
            const std::shared_ptr<model::DocraftTable> &node);

        /**
         * @brief Returns the usable width for the table.
         */
        float available_width_for(const std::shared_ptr<model::DocraftTable> &node,
                                  float fallback) const;

        /**
         * @brief Moves cursor to absolute position if table is positioned absolutely.
         */
        static void configure_cursor_position(const std::shared_ptr<model::DocraftTable> &node,
                                              DocraftCursor &cursor);

        /**
         * @brief Vertically centers text node within row.
         */
        static void center_text_in_row(const std::shared_ptr<model::DocraftNode> &node,
                                       float row_top_y,
                                       float row_height,
                                       float baseline_offset);

        /**
         * @brief Computes cell layout with temporary cursor repositioning.
         */
        void compute_cell_layout(const std::shared_ptr<model::DocraftNode> &node,
                                 float inner_width,
                                 float x,
                                 float y);

        /**
         * @brief Builds left-edge x-coordinates for each column.
         */
        static std::vector<float> build_column_lefts(float fixed_x,
                                                     const std::vector<float> &col_widths);

        /**
         * @brief Returns normalized weight vector (no zeros).
         */
        static std::vector<float> assign_weights(const std::vector<float> &source,
                                                 std::size_t cols);

        /**
         * @brief Writes final node/box geometry.
         */
        void finalize_output(const std::shared_ptr<model::DocraftTable> &node,
                             model::DocraftTransform *box,
                             float table_width,
                             float table_height);

        /**
         * @brief Emits debug logs for cell geometry.
         */
        static void log_cells(const std::shared_ptr<model::DocraftTable> &node);

        /**
         * @brief Releases temporary resources.
         */
        void clear_compute_state();

        /**
         * @brief Returns the document context.
         */
        [[nodiscard]] const std::shared_ptr<DocraftDocumentContext> &get_context() const;

        /**
         * @brief Returns the layout engine reference.
         */
        [[nodiscard]] DocraftLayoutEngine &get_engine();

        /**
         * @brief Returns the table cursor.
         */
        [[nodiscard]] DocraftCursor &get_table_cursor();

        /**
         * @brief Returns horizontal cell padding.
         */
        [[nodiscard]] float get_cell_padding_x() const;

        /**
         * @brief Returns vertical cell padding.
         */
        [[nodiscard]] float get_cell_padding_y() const;

        /**
         * @brief Returns baseline offset for text centering.
         */
        [[nodiscard]] float get_baseline_offset() const;

        /**
         * @brief Returns fixed x-coordinate of table.
         */
        [[nodiscard]] float get_fixed_x() const;

        /**
         * @brief Returns fixed y-coordinate of table.
         */
        [[nodiscard]] float get_fixed_y() const;

        /**
         * @brief Sets fixed y-coordinate of table.
         */
        void set_fixed_y(float y);

        /**
         * @brief Returns saved available space before layout.
         */
        [[nodiscard]] float get_saved_available_space() const;

        /**
         * @brief Computes the row height for a band by laying out each cell content once.
         *
         * Height is derived from the tallest content cell plus vertical paddings, then clamped
         * to the band's min_row_height.
         */
        float compute_row_height(const RowBand &band, float row_top_y);

        /**
         * @brief Writes final geometry for each cell in a row band using the resolved row height.
         */
        void place_row_band(const RowBand &band, float row_top_y, float row_height);

        /**
         * @brief Shared row pipeline used by both table orientations.
         *
         * Algorithm:
         *  1) layout each cell content in its own inner box,
         *  2) resolve a single row height,
         *  3) place every cell with aligned row geometry.
         */
        float layout_row_band(const RowBand &band, float row_top_y);

        /**
         * @brief Validates that row vectors are aligned and returns the effective cell count.
         */
        static std::size_t effective_band_size(const RowBand &band);

    private:
        std::shared_ptr<DocraftDocumentContext> context_;
        std::optional<DocraftLayoutEngine> engine_;
        DocraftCursor table_cursor_;
        float cell_padding_x_ = 0.0F;
        float cell_padding_y_ = 0.0F;
        float baseline_offset_ = 0.0F;
        float fixed_x_ = 0.0F;
        float fixed_y_ = 0.0F;
        float saved_available_space_ = 0.0F;
    };
} // docraft

//
// Created by Matteo on 27/06/2026.
//

#pragma once
#include "docraft/loom/interfaces/docraft_loom_visitor.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::pipeline {
    /**
     * @brief Cursor class for tracking the current position in the layout process.
     * Zero is the top-left corner of the page, and coordinates increase to the right and down.
     */
    class DocraftLoomCursor
    {
    public:
        DocraftLoomCursor() = default;
        ~DocraftLoomCursor() = default;
        //getter
        float x() const;
        float y() const;
        //setter
        /**
         * @brief Moves the cursor by the specified delta values in the x and y directions.
         * @param dx
         * @param dy
         */
        void move(float dx, float dy);
        /**
         * @brief Sets the cursor's position to the specified coordinates.
         * @param x
         * @param y
         */
        void set_position(float x, float y);

    private:
        const float kDefaultTopMargin_ = 10.0F;
        nodes::Position current_position_ = {0.0f, kDefaultTopMargin_};
    };

    /**
     * @brief Implementation of the DocraftLoomIVisitor interface for processing Loom nodes during the layout phase.
     *
     */
    class DocraftLoomLayoutProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        /**
         * @brief Constructs the layout processor.
         * @param page_width Width of the page in points, used to resolve block-flow width
         * (e.g. blank lines, tables) and as the container width for absolute positioning
         * fallbacks. Defaults to 0.0F (unconstrained) to preserve existing no-arg call sites.
         */
        explicit DocraftLoomLayoutProcessor(float page_width = 0.0F);

        /**
         * @brief Resets the shared cursor to an explicit position, so the same processor
         * instance can be reused across separate layout regions (header/body/footer) that
         * each need their own starting point instead of the cursor's default top margin.
         */
        void reset_cursor(float x, float y);

        /**
         * @brief Sets the width available to block-flow content in the region about to be
         * laid out (blank lines stretching to fill it, table column resolution). Callers
         * reusing the same processor across header/body/footer regions with different
         * margins call this alongside reset_cursor() before each region's layout pass.
         */
        void set_content_width(float width);

        void visit(docraft::loom::nodes::DocraftLoomText*) override;
        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomParagraph*) override;
        void visit(docraft::loom::nodes::DocraftLoomVStack*) override;
        void visit(docraft::loom::nodes::DocraftLoomHStack*) override;
        void visit(docraft::loom::nodes::DocraftLoomBlankLine*) override;
        void visit(docraft::loom::nodes::DocraftLoomImage*) override;
        void visit(docraft::loom::nodes::DocraftLoomLine*) override;
        void visit(docraft::loom::nodes::DocraftLoomCircle*) override;
        void visit(docraft::loom::nodes::DocraftLoomTriangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomPolygon*) override;
        void visit(docraft::loom::nodes::DocraftLoomList*) override;
        void visit(docraft::loom::nodes::DocraftLoomTableCell*) override;
        void visit(docraft::loom::nodes::DocraftLoomTable*) override;
        void visit(docraft::loom::nodes::DocraftLoomPageNumber*) override;
        void visit(docraft::loom::nodes::DocraftLoomNewPage*) override;

    private:
        /**
         * @brief Resolves a node's frame position: the explicit position for absolute-mode
         * nodes, or the shared cursor's current position for block-mode nodes.
         */
        nodes::Position resolve_position(const nodes::DocraftLoomNode& node) const;


        /**
         * @brief RAII helper class to manage cursor state during node visits.
         */
        class PositionScope
        {
        public:
            PositionScope(DocraftLoomLayoutProcessor& processor, nodes::DocraftLoomNode& node);
            ~PositionScope();
            PositionScope(const PositionScope&) = delete;
            PositionScope& operator=(const PositionScope&) = delete;

        private:
            DocraftLoomLayoutProcessor& processor_;
            DocraftLoomCursor entry_cursor_;
            bool restore_on_exit_;
        };

        /**
         * @brief Per-column/row geometry gathered from what Measure already computed on
         * each cell, before any column-width resolution happens.
         */
        struct TableNaturalGeometry
        {
            std::vector<float> natural_widths;
            std::vector<float> row_heights;
            std::vector<float> explicit_widths;
        };

        static TableNaturalGeometry gather_table_natural_geometry(const nodes::DocraftLoomTable& table);
        std::vector<float> resolve_table_column_widths(const nodes::DocraftLoomTable& table,
                                                       const TableNaturalGeometry& geometry) const;
        static float table_cell_horizontal_offset(const nodes::DocraftLoomNode& content, float extra_width);
        void place_table_cells(
            nodes::DocraftLoomTable& table, const std::vector<float>& resolved_widths,
            const std::vector<float>& row_heights);

        DocraftLoomCursor cursor_;
        nodes::Size page_size_; /// Document page sizes, used for layout computation
    };
} // docraft

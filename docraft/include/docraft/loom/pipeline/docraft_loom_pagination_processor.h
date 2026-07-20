//
// Created by Matteo on 27/06/2026.
//

#pragma once
#include <memory>

#include "docraft/backend/docraft_page_rendering_backend.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"
#include "docraft/loom/nodes/docraft_loom_table.h"

namespace docraft::loom::pipeline {
    class DocraftLoomPaginationProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        void visit(docraft::loom::nodes::DocraftLoomText*) override;
        void visit(docraft::loom::nodes::DocraftLoomTitle*) override;
        void visit(docraft::loom::nodes::DocraftLoomSubtitle*) override;
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

        /**
         * @brief Paginates the body's top-level children (body_root's own children_, in
         * order): each child either fits on the current page as-is, moves whole to a new
         * page, or -- if it's a DocraftLoomTable that alone doesn't fit -- gets split into
         * a fragment that stays plus a remainder table inserted as the next sibling and
         * placed on a new page (mirrors legacy's DocraftTable::split_after_row).
         *
         * Every node touched gets layout_box().page_index stamped recursively (0-based),
         * so DocraftLoomRenderingProcessor can later draw only the content assigned to
         * whichever page is currently being rendered. New physical pages are created via
         * page_backend eagerly, as soon as an overflow is detected (mirroring legacy).
         *
         * Scope note: only body_root's direct children are pagination-aware; content
         * nested deeper (e.g. a table inside a Rectangle) is not split and simply moves
         * or stays as a whole unit. An oversized child that doesn't fit even on a fresh
         * page is left overflowing rather than looping forever creating empty pages.
         *
         * @param body_root Root container of the body (typically a VStack).
         * @param body_top_y Y coordinate (continuous layout space) of the body's top on
         * every page.
         * @param body_height Height of the body region available on every page.
         * @param page_backend Used to create additional physical pages as needed.
         * @return Total number of pages the body now spans (>= 1).
         */
        int paginate_body(nodes::DocraftLoomNode& body_root, float body_top_y, float body_height,
                          backend::IDocraftPageRenderingBackend* page_backend);

        /**
         * @brief Stamps page_index on a node and every descendant, recursively (including
         * table cells, which live outside the inherited children_ vector). Pass -1 to mark
         * a subtree as "render on every page" -- used for header/footer content and for
         * the body's own root container (whose direct children carry the real per-page
         * indices assigned by paginate_body()).
         */
        static void assign_page_index_recursive(nodes::DocraftLoomNode& node, int page_index);

    private:
        static void shift_subtree_position(nodes::DocraftLoomNode& node, float dy);

        /**
         * @brief Attempts to split a table that doesn't fit in the remaining space on the
         * current page. Rows whose top is at or below page_bottom_y move to a new
         * DocraftLoomTable, re-stacked starting at new_page_top_y; any row(s) at the start
         * of the table that are entirely title cells are treated as a repeating header
         * and cloned onto the remainder too (a simplification of legacy's per-orientation
         * title handling: only a contiguous all-title prefix is treated as a header).
         * @return The remainder table, or nullptr if no split was possible/needed
         * (table has 0 or 1 rows fitting, or nothing overflows).
         */
        static std::shared_ptr<nodes::DocraftLoomTable> try_split_table(
            nodes::DocraftLoomTable& table, float page_bottom_y, float new_page_top_y);
    };
} // docraft
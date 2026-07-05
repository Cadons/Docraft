//
// Created by Matteo on 29/06/2026.
//

#pragma once
#include <memory>
#include <string>

#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/model/docraft_text.h"

namespace docraft::loom::pipeline {
    class DocraftLoomRenderingProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        explicit DocraftLoomRenderingProcessor(backend::IDocraftRenderingCapabilityProvider* backend = nullptr);
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

        /**
         * @brief Sets which physical page is currently being rendered, so that
         * should_render() gates paginated content correctly and DocraftLoomPageNumber
         * can display the right value. Call once per page, before re-running accept()
         * on the document tree for that page.
         * @param page_index 0-based index of the page currently being drawn.
         * @param total_page_count Total number of pages in the document (for "n of N").
         */
        void set_current_page(int page_index, int total_page_count);

    private:
        void draw_table_default_backgrounds(nodes::DocraftLoomTable& table);
        void draw_table_borders_and_dividers(nodes::DocraftLoomTable& table);
        void draw_table_content(nodes::DocraftLoomTable& table);

        /**
         * @brief Style shared by every wrapped line of a single DocraftLoomText -- the
         * per-line position (line text, x, y) is passed separately to draw_aligned_line
         * since it varies each call, while this stays constant across a text block's lines.
         */
        struct TextLineStyle
        {
            float box_width;
            model::TextAlignment alignment;
            std::string font_name;
            float font_size;
        };

        /**
         * @brief Draws one already-positioned line of text at (x, y), honoring
         * style.alignment within style.box_width -- kJustified distributes the gap
         * between the line's actual width and style.box_width evenly across its
         * inter-word spaces (falling back to kLeft if there are no spaces, or the line
         * already fills style.box_width).
         */
        void draw_aligned_line(const std::string& line, float x, float y, const TextLineStyle& style);

        /**
         * @brief Whether a node should be drawn during the current page's render pass:
         * true for unpaginated content (layout_box().page_index < 0, e.g. a header/footer
         * or any document that never ran real pagination) or content assigned to the
         * page currently being rendered.
         */
        bool should_render(const nodes::DocraftLoomNode& node) const;

        backend::IDocraftTextRenderingBackend* text_backend_;
        backend::IDocraftShapeRenderingBackend* shape_backend_;
        backend::IDocraftLineRenderingBackend* line_backend_;
        backend::IDocraftImageRenderingBackend* image_backend_;
        int current_page_index_ = 0;
        int total_page_count_ = 1;
    };
} // docraft

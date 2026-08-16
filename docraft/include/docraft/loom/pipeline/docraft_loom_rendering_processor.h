//
// Created by Matteo on 29/06/2026.
//

#pragma once
#include <memory>
#include <string>

#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"
#include "docraft/docraft_color.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_shape_style.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::pipeline {
    class DocraftLoomRenderingProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        explicit DocraftLoomRenderingProcessor(backend::IDocraftRenderingCapabilityProvider* backend = nullptr);
        void visit(docraft::loom::nodes::DocraftLoomText*) override;
        void visit(docraft::loom::nodes::DocraftLoomTitle*) override;
        void visit(docraft::loom::nodes::DocraftLoomSubtitle*) override;
        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomCanvas*) override;
        void visit(docraft::loom::nodes::DocraftLoomParagraph*) override;
        void visit(docraft::loom::nodes::DocraftLoomVStack*) override;
        void visit(docraft::loom::nodes::DocraftLoomHStack*) override;
        void visit(docraft::loom::nodes::DocraftLoomBlankLine*) override;
        void visit(docraft::loom::nodes::DocraftLoomImage*) override;
        void visit(docraft::loom::nodes::DocraftLoomLine*) override;
        void visit(docraft::loom::nodes::DocraftLoomCircle*) override;
        void visit(docraft::loom::nodes::DocraftLoomTriangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomPolygon*) override;
        void visit(docraft::loom::nodes::DocraftLoomCurveLine*) override;
        void visit(docraft::loom::nodes::DocraftLoomList*) override;
        void visit(docraft::loom::nodes::DocraftLoomTableCell*) override;
        void visit(docraft::loom::nodes::DocraftLoomTable*) override;
        void visit(docraft::loom::nodes::DocraftLoomPageNumber*) override;
        void visit(docraft::loom::nodes::DocraftLoomNewPage*) override;

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
            nodes::TextAlignment alignment;
            std::string font_name;
            float font_size;
            bool underline = false;
            bool strikeout = false;
            RGB color;
        };

        /**
         * @brief Horizontal extent [x_start, x_end] a drawn line of text actually
         * occupies, as positioned by draw_aligned_line's alignment handling -- used to
         * size an underline stroke to the visible text rather than the full box_width.
         */
        struct LineExtent
        {
            float x_start;
            float x_end;
        };

        /**
         * @brief Draws one already-positioned line of text at (x, y), honoring
         * style.alignment within style.box_width -- kJustified distributes the gap
         * between the line's actual width and style.box_width evenly across its
         * inter-word spaces (falling back to kLeft if there are no spaces, or the line
         * already fills style.box_width). Returns the horizontal extent drawn, for
         * callers that need to underline it.
         */
        LineExtent draw_aligned_line(const std::string& line, float x, float y, const TextLineStyle& style);

        /**
         * @brief Strokes an underline from (x_start, y_baseline) to (x_end, y_baseline),
         * offset below the baseline using descent (see DocraftLoomText::descent()) and
         * sized proportionally to font_size. Must be called outside a
         * begin_text()/end_text() block -- path painting operators are not valid inside
         * a PDF text object.
         */
        void draw_text_underline(float x_start, float x_end, float y_baseline, float font_size, float descent,
                                 const RGB& color);

        /**
         * @brief Strokes a strikethrough from (x_start, y_baseline) to (x_end,
         * y_baseline), offset above the baseline using ascent (see
         * DocraftLoomText::ascent()) and sized proportionally to font_size. Must be
         * called outside a begin_text()/end_text() block -- path painting operators are
         * not valid inside a PDF text object.
         */
        void draw_text_strikeout(float x_start, float x_end, float y_baseline, float font_size, float ascent,
                                 const RGB& color);

        /**
         * @brief Shared stroke primitive for draw_text_underline/draw_text_strikeout --
         * both are a single horizontal rule at a caller-computed y, differing only in how
         * that y is derived from the text's metrics.
         */
        void draw_horizontal_text_rule(float x_start, float x_end, float y, float thickness, const RGB& color);

        /**
         * @brief Fraction of descent()'s magnitude used as the underline's offset below
         * the baseline (see draw_text_underline).
         */
        static constexpr float kUnderlineDescentFraction = 1.0F / 3.0F;
        /**
         * @brief Fraction of ascent() used as the strikeout's offset above the baseline
         * (see draw_text_strikeout).
         */
        static constexpr float kStrikeoutAscentFraction = 1.0F / 3.0F;
        /**
         * @brief Fraction of font_size used as underline/strikeout stroke thickness,
         * floored by kTextRuleMinThickness (see draw_horizontal_text_rule callers).
         */
        static constexpr float kTextRuleThicknessFontSizeFraction = 1.0F / 18.0F;
        /**
         * @brief Minimum underline/strikeout stroke thickness in points, regardless of
         * font_size.
         */
        static constexpr float kTextRuleMinThickness = 0.75F;

        /**
         * @brief Paints style's background fill and/or border stroke as a rectangle at
         * (position, size), or does nothing if style has neither set. Shared by every
         * DocraftLoomLayoutContainer subclass's own visit() (Rectangle, VStack, HStack)
         * so the save_state()/apply paint state/draw/finish/restore_state() sequence
         * isn't duplicated at each call site.
         */
        void draw_container_background(const nodes::DocraftLoomShapeStyle& style, const nodes::Position& position,
                                       const nodes::Size& size);

        /**
         * @brief Paints node's children (in paint_order_indices() order) clipped to
         * frame, so nothing a child paints can escape those bounds. Shared by every
         * container that must contain its children (Rectangle, Canvas) so the
         * save_state()/clip_rectangle()/.../restore_state() sequence isn't duplicated
         * at each call site.
         */
        void paint_children_clipped_to_frame(nodes::DocraftLoomNode& node, const nodes::Rect& frame);

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

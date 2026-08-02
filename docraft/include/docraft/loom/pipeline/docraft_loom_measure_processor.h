/**
 * @file docraft/loom/pipeline/docraft_loom_measure_processor.h
 *
 * @brief Defines the DocraftLoomMeasureProcessor class, which is an implementation of the DocraftLoomIVisitor interface for measuring Loom nodes.
 *
 * The DocraftLoomMeasureProcessor class is responsible for measuring various types of Loom nodes during the layout process. This includes:
 * - docraft::loom::nodes::DocraftLoomText
 * - docraft::loom::nodes::DocraftLoomRectangle
 *
 * @author Matteo Cadoni (https://github.com/cadons)
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "docraft/backend/docraft_backend_providers_factory.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"

namespace docraft::loom::pipeline {
    /**
     * @class DocraftLoomMeasureProcessor
     *
     * Implementation of the DocraftLoomIVisitor interface for measuring Loom nodes.
     */
    class DocraftLoomMeasureProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        /**
         * @brief Constructor for the DocraftLoomMeasureProcessor class.
         */
        DocraftLoomMeasureProcessor(const std::shared_ptr<backend::IDocraftTextRenderingBackend>& text_backend);

        /**
         * @brief Sets the width available to block-flow content for the region about to
         * be measured (mirrors DocraftLoomLayoutProcessor::set_content_width) -- needed
         * so a weighted DocraftLoomHStack can resolve each column's width and push it
         * down to Text descendants as their wrap width, all before Layout ever runs. Also
         * clears any wrap-width constraint left over from a previous region's traversal
         * (see DocraftLoomPdfCreator::create(), which reuses one processor instance
         * across header/footer/body): each region must start fresh, with no ancestor-
         * pushed constraint carried over from an unrelated region.
         */
        void set_content_width(float width);

        /**
         * @brief Destructor for the DocraftLoomMeasureProcessor class.
         *
         * This destructor is defaulted and does not need to be overridden.
         */
        ~DocraftLoomMeasureProcessor() override = default;

        void visit(docraft::loom::nodes::DocraftLoomText*) override;

        /**
         * @brief Measured exactly like ordinary text (see visit(DocraftLoomText*)) --
         * DocraftLoomTitle/DocraftLoomSubtitle only differ in their constructor's
         * default font/style/margin.
         */
        void visit(docraft::loom::nodes::DocraftLoomTitle*) override;
        void visit(docraft::loom::nodes::DocraftLoomSubtitle*) override;

        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomCanvas*) override;

        /**
         * @brief Measures the paragraph as the sum of its children's heights plus
         * inter-line spacing, plus space_before and space_after.
         */
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
         * @brief Greedily word-wraps text into lines that each fit within max_width,
         * falling back to a character-level split for a single word wider than
         * max_width on its own. Splits on explicit '\n' first, then wraps each resulting
         * paragraph by words.
         */
        std::vector<std::string> wrap_text(const std::string& text, float max_width, const std::string& font_name,
                                           float font_size) const;

        std::shared_ptr<docraft::backend::IDocraftTextRenderingBackend> text_backend_;
        float content_width_ = 0.0F;

        // The width budget the node about to be measured should wrap/fit into, pushed
        // down by whichever ancestor last narrowed it: Rectangle/VStack relay
        // content_width_ (or their own explicit width, minus padding) to every child,
        // a weighted HStack resolves each column's share, List narrows it by each
        // item's marker prefix, and Paragraph passes it through unchanged to each of
        // its own children in turn. Consumed (and cleared) by the next DocraftLoomText
        // that measures without its own explicit wrap_width. HStack (unweighted) and
        // Table clear it on entry since they resolve widths their own way instead.
        // Zero means "no inherited constraint".
        float inherited_wrap_width_ = 0.0F;

        // Pushed by DocraftLoomTable onto the cell it's about to measure: a best-effort
        // estimate of that column's final width (explicit_width(), a weight-based share,
        // or an even split of content_width_), consumed by visit(DocraftLoomTableCell*).
        // Unlike inherited_wrap_width_ (which unconditionally stretches a weighted
        // HStack child to fill its column), this is only a wrap *ceiling*: a cell whose
        // natural text width already fits keeps its natural (shrink-to-fit) width, so
        // Table's existing natural-width-floor column sizing isn't disturbed for the
        // common case of short cell content. Zero means "no budget known yet".
        float pending_cell_wrap_budget_ = 0.0F;
    };
} // namespace docraft::loom::pipeline

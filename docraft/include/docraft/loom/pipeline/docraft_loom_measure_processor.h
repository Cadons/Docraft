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
         * down to Text descendants as their wrap width, all before Layout ever runs.
         */
        void set_content_width(float width);

        /**
         * @brief Destructor for the DocraftLoomMeasureProcessor class.
         *
         * This destructor is defaulted and does not need to be overridden.
         */
        ~DocraftLoomMeasureProcessor() override = default;

        /**
         * @brief Visits a doccraft::loom::nodes::DocraftLoomText node during the measurement process.
         *
         * This method computes any necessary measurements for the DocraftLoomText node, such as its width and height, or
         * adjusts layout properties based on the text content. It is called when visiting a DocraftLoomText node in the Loom graph.
         *
         * @param text_node Pointer to the doccraft::loom::nodes::DocraftLoomText node being visited.
         */
        void visit(docraft::loom::nodes::DocraftLoomText*) override;

        /**
         * @brief Visits a doccraft::loom::nodes::DocraftLoomRectangle node during the measurement process.
         *
         * This method computes any necessary measurements for the DocraftLoomRectangle node, such as its width and height, or
         * adjusts layout properties based on the rectangle's content. It is called when visiting a DocraftLoomRectangle node in the Loom graph.
         *
         * @param rect_node Pointer to the doccraft::loom::nodes::DocraftLoomRectangle node being visited.
         */
        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;

        /**
         * @brief Visits a DocraftLoomParagraph node during the measurement process.
         *
         * Measures the paragraph as the sum of its children's heights plus inter-line spacing,
         * plus space_before and space_after.
         *
         * @param paragraph Pointer to the DocraftLoomParagraph node being visited.
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

        // Pushed by a weighted HStack onto the child it's about to measure (its
        // resolved column width), consumed by the next DocraftLoomText that measures
        // without its own explicit wrap_width -- Paragraph passes it through unchanged
        // to each of its own children in turn. Zero means "no inherited constraint".
        float inherited_wrap_width_ = 0.0F;
    };
} // namespace docraft::loom::pipeline

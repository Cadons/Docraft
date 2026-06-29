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

    private:
        std::shared_ptr<docraft::backend::IDocraftTextRenderingBackend> text_backend_;
    };
} // namespace docraft::loom::pipeline

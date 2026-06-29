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
        explicit DocraftLoomLayoutProcessor();
        void visit(docraft::loom::nodes::DocraftLoomText*) override;
        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomParagraph*) override;
        void visit(docraft::loom::nodes::DocraftLoomVStack*) override;
        void visit(docraft::loom::nodes::DocraftLoomHStack*) override;

    private:
        DocraftLoomCursor cursor_;
        nodes::Size page_size_; /// Document page sizes, used for layout computation
    };
} // docraft

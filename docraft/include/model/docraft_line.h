#pragma once

#include "docraft_color.h"
#include "docraft_node.h"
#include "model/docraft_position.h"
#include "model/i_docraft_clonable.h"

namespace docraft::model {
    /**
     * @brief Line node defined by start and end points.
     */
    class DocraftLine : public DocraftNode, public IDocraftClonable {
    public:
        using DocraftNode::DocraftNode;
        /**
         * @brief Draws the line using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        /**
         * @brief Clones the line node.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
        /**
         * @brief Sets the line start point (local coordinates).
         * @param point Start point.
         */
        void set_start(const DocraftPoint &point);
        /**
         * @brief Sets the line end point (local coordinates).
         * @param point End point.
         */
        void set_end(const DocraftPoint &point);
        /**
         * @brief Sets the stroke color.
         * @param color Stroke color.
         */
        void set_border_color(const DocraftColor &color);
        /**
         * @brief Sets the stroke width.
         * @param width Stroke width in points.
         */
        void set_border_width(float width);
        /**
         * @brief Returns the start point.
         * @return Start point.
         */
        const DocraftPoint &start() const;
        /**
         * @brief Returns the end point.
         * @return End point.
         */
        const DocraftPoint &end() const;
        /**
         * @brief Returns the stroke color.
         * @return Stroke color.
         */
        const DocraftColor &border_color() const;
        /**
         * @brief Returns the stroke width in points.
         * @return Stroke width.
         */
        float border_width() const;
    private:
        DocraftPoint start_{};
        DocraftPoint end_{};
        DocraftColor border_color_ = DocraftColor(0, 0, 0, 0);
        float border_width_ = 1.0F;
    };
} // docraft::model

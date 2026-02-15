#pragma once

#include <vector>

#include "docraft_color.h"
#include "docraft_node.h"
#include "model/docraft_position.h"

namespace docraft::model {
    /**
     * @brief Triangle node defined by three points.
     */
    class DocraftTriangle : public DocraftNode {
    public:
        using DocraftNode::DocraftNode;
        /**
         * @brief Draws the triangle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        /**
         * @brief Sets the triangle points (local coordinates).
         * @param points Points vector (size must be 3).
         */
        void set_points(const std::vector<DocraftPoint> &points);
        /**
         * @brief Returns the triangle points.
         * @return Points vector.
         */
        const std::vector<DocraftPoint> &points() const;
        /**
         * @brief Sets the background color.
         * @param color Background color.
         */
        void set_background_color(const DocraftColor &color);
        /**
         * @brief Sets the border color.
         * @param color Border color.
         */
        void set_border_color(const DocraftColor &color);
        /**
         * @brief Sets the border width in points.
         * @param width Border width.
         */
        void set_border_width(float width);
        /**
         * @brief Returns the background color.
         * @return Background color.
         */
        const DocraftColor &background_color() const;
        /**
         * @brief Returns the border color.
         * @return Border color.
         */
        const DocraftColor &border_color() const;
        /**
         * @brief Returns the border width in points.
         * @return Border width.
         */
        float border_width() const;
    private:
        std::vector<DocraftPoint> points_{};
        DocraftColor background_color_ = DocraftColor(0, 0, 0, 0);
        DocraftColor border_color_ = DocraftColor(0, 0, 0, 0);
        float border_width_ = 1.0F;
    };
} // docraft::model

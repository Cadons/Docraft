#pragma once

#include "docraft_color.h"
#include "docraft_node.h"
#include "model/i_docraft_clonable.h"

namespace docraft::model {
    /**
     * @brief Circle node with background and border styling.
     */
    class DocraftCircle : public DocraftNode, public IDocraftClonable {
    public:
        using DocraftNode::DocraftNode;
        /**
         * @brief Draws the circle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        /**
         * @brief Clones the circle node.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
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
        DocraftColor background_color_ = DocraftColor(0, 0, 0, 0); // transparent
        DocraftColor border_color_ = DocraftColor(0, 0, 0, 0);
        float border_width_ = 1.0F;
    };
} // docraft::model

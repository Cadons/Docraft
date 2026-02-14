#pragma once
#include "docraft_children_container_node.h"
#include "docraft_color.h"
#include "docraft_node.h"

namespace docraft::model {
    /**
     * @brief Rectangle node with background and border styling.
     *
     * Often used as a container to draw a box behind its children.
     */
    class DocraftRectangle : public DocraftChildrenContainerNode {
    public:
        using DocraftChildrenContainerNode::DocraftChildrenContainerNode;
        /**
         * @brief Draws the rectangle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
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
         * @param width Border width in points.
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
         * @return Border width in points.
         */
        float border_width() const;


    private:
        DocraftColor background_color_= DocraftColor(0,0,0,0); //transparent;
        DocraftColor border_color_= DocraftColor(0,0,0,0);
        float border_width_= 1.0F;
    };
} // docraft

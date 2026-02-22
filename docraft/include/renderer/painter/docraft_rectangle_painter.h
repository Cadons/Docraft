#pragma once

#include "docraft_lib.h"
#include "model/docraft_rectangle.h"
#include "renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftRectangle nodes.
     *
     * Renders background and border before child content.
     */
    class DOCRAFT_LIB DocraftRectanglePainter :public IPainter{
    public:
        /**
         * @brief Creates a rectangle painter bound to the rectangle node.
         * @param rectangle_node Rectangle node.
         */
        DocraftRectanglePainter(const model::DocraftRectangle &rectangle_node);
        /**
         * @brief Draws the rectangle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        model::DocraftRectangle rectangle_node_;
    };
} // docraft

#pragma once

#include "docraft_lib.h"
#include "model/docraft_triangle.h"
#include "renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftTriangle nodes.
     */
    class DOCRAFT_LIB DocraftTrianglePainter : public IPainter {
    public:
        /**
         * @brief Creates a triangle painter bound to the triangle node.
         * @param triangle_node Triangle node.
         */
        explicit DocraftTrianglePainter(const model::DocraftTriangle &triangle_node);
        /**
         * @brief Draws the triangle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        model::DocraftTriangle triangle_node_;
    };
} // docraft::renderer::painter

#pragma once
#include "model/docraft_polygon.h"
#include "renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftPolygon nodes.
     */
    class DocraftPolygonPainter : public IPainter {
    public:
        /**
         * @brief Creates a polygon painter bound to the polygon node.
         * @param polygon_node Polygon node.
         */
        explicit DocraftPolygonPainter(const model::DocraftPolygon &polygon_node);
        /**
         * @brief Draws the polygon using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        model::DocraftPolygon polygon_node_;
    };
} // docraft::renderer::painter

#pragma once

#include "docraft_lib.h"
#include "model/docraft_circle.h"
#include "renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftCircle nodes.
     */
    class DOCRAFT_LIB DocraftCirclePainter : public IPainter {
    public:
        /**
         * @brief Creates a circle painter bound to the circle node.
         * @param circle_node Circle node.
         */
        explicit DocraftCirclePainter(const model::DocraftCircle &circle_node);
        /**
         * @brief Draws the circle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        model::DocraftCircle circle_node_;
    };
} // docraft::renderer::painter

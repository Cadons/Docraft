#pragma once

#include "docraft_lib.h"
#include "model/docraft_line.h"
#include "renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftLine nodes.
     */
    class DOCRAFT_LIB DocraftLinePainter : public IPainter {
    public:
        /**
         * @brief Creates a line painter bound to the line node.
         * @param line_node Line node.
         */
        explicit DocraftLinePainter(const model::DocraftLine &line_node);
        /**
         * @brief Draws the line using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        model::DocraftLine line_node_;
    };
} // docraft::renderer::painter

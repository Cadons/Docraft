#pragma once
#include "model/docraft_rectangle.h"
#include "renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    class DocraftRectanglePainter :public IPainter{
    public:
        DocraftRectanglePainter(const model::DocraftRectangle &rectangle_node);
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        model::DocraftRectangle rectangle_node_;
    };
} // docraft

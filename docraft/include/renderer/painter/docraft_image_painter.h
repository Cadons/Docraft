#pragma once
#include "i_painter.h"
#include "model/docraft_image.h"

namespace docraft::renderer::painter {

    class DocraftImagePainter : public renderer::painter::IPainter{
        public:
        explicit DocraftImagePainter(const model::DocraftImage &image_node);
        void draw(const std::shared_ptr<DocraftPDFContext> &context) override;

        private:
        const model::DocraftImage &image_node_;
    };
} // docraft

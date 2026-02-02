#pragma once
#include "i_painter.h"
#include "model/docraft_blank_line.h"

namespace docraft::renderer::painter {
    class docraft_blank_line_painter : public renderer::painter::IPainter {
    public:
        explicit docraft_blank_line_painter(const model::DocraftBlankLine &blank_line_node);

        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    private:
        const model::DocraftBlankLine &blank_line_node_;
    };
} // docraft

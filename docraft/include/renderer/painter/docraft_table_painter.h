#pragma once
#include "i_painter.h"
#include "model/docraft_table.h"

namespace docraft::renderer::painter {
    class DocraftTablePainter : public renderer::painter::IPainter {
    public:
        DocraftTablePainter(const model::DocraftTable &table_node);

        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        const model::DocraftTable &table_node_;
    };
} // docraft

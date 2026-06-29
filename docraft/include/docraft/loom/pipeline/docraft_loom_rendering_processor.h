//
// Created by Matteo on 29/06/2026.
//

#pragma once
#include <memory>

#include "docraft/backend/docraft_text_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"

namespace docraft::loom::pipeline {
    class DocraftLoomRenderingProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        DocraftLoomRenderingProcessor(backend::IDocraftTextRenderingBackend* text_backend = nullptr);
        void visit(docraft::loom::nodes::DocraftLoomText*) override;
        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomParagraph*) override;

    private:
        backend::IDocraftTextRenderingBackend* text_backend_;
    };
} // docraft

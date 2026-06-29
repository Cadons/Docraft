//
// Created by Matteo on 27/06/2026.
//

#pragma once
#include "docraft/loom/interfaces/docraft_loom_visitor.h"

namespace docraft::loom::pipeline {
    class DocraftLoomPaginationProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        void visit(docraft::loom::nodes::DocraftLoomText*) override;
        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;
        void visit(docraft::loom::nodes::DocraftLoomParagraph*) override;
    };
} // docraft

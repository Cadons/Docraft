//
// Created by Matteo on 21/06/2026.
//

#pragma once
#include "docraft/loom/interfaces/docraft_loom_measure_visitor.h"

namespace docraft::loom::pipeline {
    class DocraftLoomMeasureProcessor : public interfaces::DocraftLoomIVisitor
    {
    public:
        DocraftLoomMeasureProcessor();
        ~DocraftLoomMeasureProcessor() override = default;
        void visit(docraft::loom::nodes::DocraftLoomText*) override;
        void visit(docraft::loom::nodes::DocraftLoomRectangle*) override;
    };
} // docraft

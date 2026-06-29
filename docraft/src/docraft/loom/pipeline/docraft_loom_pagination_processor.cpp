//
// Created by Matteo on 27/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"

#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomText*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        if (!node)
        {
            return;
        }
        for (int i = 0; i < node->children_count(); ++i)
        {
            if (auto child = node->edit_child(i))
            {
                child->accept(*this);
            }
        }
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node)
        {
            return;
        }
        for (int i = 0; i < node->children_count(); ++i)
        {
            if (auto child = node->edit_child(i))
            {
                child->accept(*this);
            }
        }
    }
} // pipeline

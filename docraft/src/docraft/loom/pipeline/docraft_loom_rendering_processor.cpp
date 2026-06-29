//
// Created by Matteo on 29/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"

#include "docraft/backend/pdf/docraft_haru_text_backend.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
    DocraftLoomRenderingProcessor::DocraftLoomRenderingProcessor(
        docraft::backend::IDocraftTextRenderingBackend* text_backend)
        : text_backend_(text_backend)
    {
    }


    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomText* text)
    {
        //set font
        text_backend_->set_font(text->font_family(), text->font_size());
        text_backend_->begin_text();
        text_backend_->draw_text(text->text(), text->layout_box().frame.position.x,
                                 text->layout_box().frame.position.y);
        text_backend_->end_text();
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* rectangle)
    {
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* paragraph)
    {
        for (int i = 0; i < paragraph->children_count(); ++i)
        {
            if (paragraph->edit_child(i))
                paragraph->edit_child(i)->accept(*this);
        }
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        if (!node) return;
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node) return;
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
    }
} // docraft

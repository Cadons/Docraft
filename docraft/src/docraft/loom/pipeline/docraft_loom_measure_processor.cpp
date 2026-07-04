//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

#include <algorithm>

#include "docraft/generic/docraft_font_applier.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
    DocraftLoomMeasureProcessor::DocraftLoomMeasureProcessor(
        const std::shared_ptr<backend::IDocraftTextRenderingBackend>& text_backend)
        : text_backend_(text_backend)
    {
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomText* text)
    {
        if (text)
        {
            const char* rn = generic::DocraftFontApplier::get_font_registred_name(text->font_family());
            const std::string reg_font = rn ? rn : text->font_family();
            const float font_size = text->font_size();

            auto& measure_size = text->edit_layout_box().measured_size;
            measure_size.width = text_backend_->measure_text_width(text->text(), reg_font, font_size);
            measure_size.height = text_backend_->measure_text_height(reg_font, font_size);
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* rectangle)
    {
        if (rectangle)
        {
            // Perform measurement logic for DocraftLoomRectangle node
            auto& measured_size = rectangle->


        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* paragraph)
    {
        if (!paragraph)
            return;

        float total_height = paragraph->space_before() + paragraph->space_after(); // Start with space before and after
        float max_width = 0.0f;

        for (int i = 0; i < paragraph->children_count(); ++i)
        {
            auto child = paragraph->edit_child(i);
            child->accept(*this);
            const auto& child_size = child->layout_box().measured_size;
            total_height += child_size.height * paragraph->line_spacing();
            // Update max_width if the child's width is greater
            if (child_size.width > max_width)
                max_width = child_size.width;
        }
        // Set the measured size of the paragraph node
        auto& measured_size = paragraph->edit_layout_box().measured_size;
        measured_size.width = max_width;
        measured_size.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        if (!node) return;
        float total_height = 0.0F;
        float max_width = 0.0F;
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto& sz = child->layout_box().measured_size;
            total_height += sz.height;
            if (i < n - 1)
            {
                total_height += node->spacing();
            }
            max_width = std::max(max_width, sz.width);
        }
        auto& ms = node->edit_layout_box().measured_size;
        ms.width = max_width;
        ms.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node) return;
        float total_width = 0.0F;
        float max_height = 0.0F;
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto& sz = child->layout_box().measured_size;
            total_width += sz.width;
            if (i < n - 1)
            {
                total_width += node->spacing();
            }
            max_height = std::max(max_height, sz.height);
        }
        auto& ms = node->edit_layout_box().measured_size;
        ms.width = total_width;
        ms.height = max_height;
    }
} // docraft

#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"

#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
#pragma region Cursor

    float DocraftLoomCursor::x() const
    {
        return current_position_.x;
    }

    float DocraftLoomCursor::y() const
    {
        return current_position_.y;
    }

    void DocraftLoomCursor::move(float dx, float dy)
    {
        current_position_.x += dx;
        current_position_.y += dy;
    }

    void DocraftLoomCursor::set_position(float x, float y)
    {
        current_position_.x = x;
        current_position_.y = y;
    }

#pragma endregion

    DocraftLoomLayoutProcessor::DocraftLoomLayoutProcessor()
    {
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomText* node)
    {
        // Implement layout logic for DocraftLoomText node, after measurement this step will place the text in the
        // correct position based on its measured size and any layout rules.
        auto& layout_box = node->edit_layout_box();
        auto& position = layout_box.frame.position;
        position.x = cursor_.x();
        position.y = cursor_.y();
        cursor_.move(layout_box.measured_size.width, 0.0f); // Move cursor to the right after placing the text
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle*)
    {
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* node)
    {
        auto& layout_box = node->edit_layout_box();
        auto& position = layout_box.frame.position;
        position.x = cursor_.x();
        position.y = cursor_.y();
        //layout the children of the paragraph
        const float start_x = position.x;
        for (int i = 0; i < node->children_count(); ++i)
        {
            auto child = node->edit_child(i);
            child->accept(*this);
            float next_y = position.y + child->layout_box().measured_size.height * node->line_spacing();
            cursor_.set_position(start_x, cursor_.y() + next_y);
        }
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        if (!node) return;
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.position = {cursor_.x(), cursor_.y()};

        const float start_x = cursor_.x();
        float current_y = cursor_.y();
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            cursor_.set_position(start_x, current_y);
            auto child = node->edit_child(i);
            child->accept(*this);
            current_y += child->layout_box().measured_size.height;
            if (i < n - 1)
            {
                current_y += node->spacing();
            }
        }
        cursor_.set_position(start_x, current_y);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node) return;
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.position = {cursor_.x(), cursor_.y()};

        const float start_y = cursor_.y();
        float current_x = cursor_.x();
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            cursor_.set_position(current_x, start_y);
            auto child = node->edit_child(i);
            child->accept(*this);
            current_x += child->layout_box().measured_size.width;
            if (i < n - 1)
            {
                current_x += node->spacing();
            }
        }
        cursor_.set_position(current_x, start_y);
    }
} // pipeline

#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"

#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

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
        auto& layoutBox = node->edit_layout_box();
        auto& position = layoutBox.frame.position;
        position.x = cursor_.x();
        position.y = cursor_.y();
        cursor_.move(layoutBox.measured_size.width, 0.0f); // Move cursor to the right after placing the text
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle*)
    {
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* node)
    {
        auto& layoutBox = node->edit_layout_box();
        auto& position = layoutBox.frame.position;
        position.x = cursor_.x();
        position.y = cursor_.y();
        //layout the children of the paragraph
        const float start_x = position.x;
        for (int i = 0; i < node->children_count(); ++i)
        {
            auto child = node->edit_child(i);
            child->accept(*this);
            float nextY = position.y + child->layout_box().measured_size.height * node->line_spacing();
            cursor_.set_position(start_x, cursor_.y() + nextY);
        }
    }
} // pipeline

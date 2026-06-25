#include "docraft/loom/nodes/docraft_loom_paragraph.h"

namespace docraft::loom::nodes {
    DocraftLoomParagraph::DocraftLoomParagraph()
        : line_spacing_(1.2f),
          space_before_(0.0f),
          space_after_(0.0f),
          alignment_(model::TextAlignment::kLeft)
    {
    }

    void DocraftLoomParagraph::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    float DocraftLoomParagraph::line_spacing() const { return line_spacing_; }
    void DocraftLoomParagraph::set_line_spacing(float line_spacing) { line_spacing_ = line_spacing; }

    float DocraftLoomParagraph::space_before() const { return space_before_; }
    void DocraftLoomParagraph::set_space_before(float space_before) { space_before_ = space_before; }

    float DocraftLoomParagraph::space_after() const { return space_after_; }
    void DocraftLoomParagraph::set_space_after(float space_after) { space_after_ = space_after; }

    model::TextAlignment DocraftLoomParagraph::alignment() const { return alignment_; }
    void DocraftLoomParagraph::set_alignment(model::TextAlignment alignment) { alignment_ = alignment; }
} // docraft::loom::nodes
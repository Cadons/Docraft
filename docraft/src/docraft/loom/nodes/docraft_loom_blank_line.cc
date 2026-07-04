#include "docraft/loom/nodes/docraft_loom_blank_line.h"

namespace docraft::loom::nodes {
    void DocraftLoomBlankLine::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    float DocraftLoomBlankLine::height() const
    {
        return requested_height_;
    }

    void DocraftLoomBlankLine::set_height(float height)
    {
        requested_height_ = height;
    }
} // docraft
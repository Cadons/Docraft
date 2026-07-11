#include "docraft/loom/nodes/docraft_loom_subtitle.h"

namespace docraft::loom::nodes {
    DocraftLoomSubtitle::DocraftLoomSubtitle()
    {
        set_font_size(18.0F);
        set_bold(true);

        set_margin(font_size());
    }

    void DocraftLoomSubtitle::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }
} // docraft
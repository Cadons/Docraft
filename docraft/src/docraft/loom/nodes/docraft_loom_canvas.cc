//
// Created by Matteo on 31/07/2026.
//

#include "docraft/loom/nodes/docraft_loom_canvas.h"

namespace docraft::loom::nodes {
    DocraftLoomCanvas::DocraftLoomCanvas()
    {
        // Padding has no meaning for freely-positioned children (their coordinates are
        // always exactly relative to frame.position), so the inherited layout-container
        // default (DocraftLoomLayoutContainer::kDefaultPadding) is neutralized here rather
        // than silently ignored-but-nonzero.
        set_padding(0.0F);
    }

    void DocraftLoomCanvas::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }
} // docraft

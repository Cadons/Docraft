#include "docraft/loom/nodes/docraft_loom_title.h"

namespace docraft::loom::nodes {
    DocraftLoomTitle::DocraftLoomTitle()
    {
        set_font_size(24.0F);
        set_bold(true);
        // 1em margin-bottom -- the standard heading convention -- so spacing stays
        // coherent with font_size instead of a fixed constant. DocraftLoomTreeBuilder::
        // build_title recomputes this from the final font_size (constructor default or
        // an explicit font-size attribute) after fill_text_node runs, so this call only
        // matters for a DocraftLoomTitle constructed directly, outside the tree builder.
        set_margin(font_size());
    }

    void DocraftLoomTitle::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }
} // docraft
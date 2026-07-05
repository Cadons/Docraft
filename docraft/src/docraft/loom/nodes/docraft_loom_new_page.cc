#include "docraft/loom/nodes/docraft_loom_new_page.h"

namespace docraft::loom::nodes {
    void DocraftLoomNewPage::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }
} // docraft

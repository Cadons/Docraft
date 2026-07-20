#include "docraft/loom/nodes/docraft_loom_page_number.h"

namespace docraft::loom::nodes {
    DocraftLoomPageNumber::DocraftLoomPageNumber() : DocraftLoomText("99999")
    {
    }

    void DocraftLoomPageNumber::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }
} // docraft

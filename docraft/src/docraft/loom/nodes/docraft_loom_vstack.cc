#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::nodes {
    DocraftLoomVStack::DocraftLoomVStack()
        : DocraftLoomLayoutContainer(12.0F)
    {
    }

    void DocraftLoomVStack::accept(interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }
} // namespace docraft::loom::nodes
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::nodes {
    void DocraftLoomVStack::accept(interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    float DocraftLoomVStack::spacing() const
    {
        return spacing_;
    }

    void DocraftLoomVStack::set_spacing(float spacing)
    {
        spacing_ = spacing;
    }
} // namespace docraft::loom::nodes

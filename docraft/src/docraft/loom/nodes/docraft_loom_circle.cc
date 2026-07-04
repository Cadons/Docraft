#include "docraft/loom/nodes/docraft_loom_circle.h"

namespace docraft::loom::nodes {
    void DocraftLoomCircle::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    float DocraftLoomCircle::radius() const
    {
        return radius_;
    }

    void DocraftLoomCircle::set_radius(float radius)
    {
        radius_ = radius;
    }
} // docraft
#include "docraft/loom/nodes/docraft_loom_circle.h"

namespace docraft::loom::nodes {
    void DocraftLoomCircle::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    float DocraftLoomCircle::radius_x() const
    {
        return radius_x_;
    }

    float DocraftLoomCircle::radius_y() const
    {
        return radius_y_;
    }

    void DocraftLoomCircle::set_radius(float radius)
    {
        radius_x_ = radius;
        radius_y_ = radius;
    }

    void DocraftLoomCircle::set_radii(float radius_x, float radius_y)
    {
        radius_x_ = radius_x;
        radius_y_ = radius_y;
    }

    bool DocraftLoomCircle::is_circle() const
    {
        return radius_x_ == radius_y_;
    }
} // docraft

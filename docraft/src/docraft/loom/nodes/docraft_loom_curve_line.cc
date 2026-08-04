#include "docraft/loom/nodes/docraft_loom_curve_line.h"

namespace docraft::loom::nodes {
    DocraftLoomCurveLine::DocraftLoomCurveLine()
        : border_color_(DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 1.0F))
    {
    }

    void DocraftLoomCurveLine::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const std::vector<Position>& DocraftLoomCurveLine::points() const
    {
        return points_;
    }

    void DocraftLoomCurveLine::set_points(const std::vector<Position>& points)
    {
        points_ = points;
    }

    const DocraftColor& DocraftLoomCurveLine::border_color() const
    {
        return border_color_;
    }

    void DocraftLoomCurveLine::set_border_color(const DocraftColor& color)
    {
        border_color_ = color;
    }

    float DocraftLoomCurveLine::border_width() const
    {
        return border_width_;
    }

    void DocraftLoomCurveLine::set_border_width(float width)
    {
        border_width_ = width;
    }
} // docraft

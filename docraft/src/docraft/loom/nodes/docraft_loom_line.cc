#include "docraft/loom/nodes/docraft_loom_line.h"

namespace docraft::loom::nodes {
    DocraftLoomLine::DocraftLoomLine()
        : border_color_(DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 1.0F))
    {
    }

    void DocraftLoomLine::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const model::DocraftPoint& DocraftLoomLine::start() const
    {
        return start_;
    }

    void DocraftLoomLine::set_start(const model::DocraftPoint& start)
    {
        start_ = start;
    }

    const model::DocraftPoint& DocraftLoomLine::end() const
    {
        return end_;
    }

    void DocraftLoomLine::set_end(const model::DocraftPoint& end)
    {
        end_ = end;
    }

    const DocraftColor& DocraftLoomLine::border_color() const
    {
        return border_color_;
    }

    void DocraftLoomLine::set_border_color(const DocraftColor& color)
    {
        border_color_ = color;
    }

    float DocraftLoomLine::border_width() const
    {
        return border_width_;
    }

    void DocraftLoomLine::set_border_width(float width)
    {
        border_width_ = width;
    }
} // docraft
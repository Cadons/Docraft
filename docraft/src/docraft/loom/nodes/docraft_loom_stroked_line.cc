#include "docraft/loom/nodes/docraft_loom_stroked_line.h"

namespace docraft::loom::nodes {
    DocraftLoomStrokedLine::DocraftLoomStrokedLine()
        : border_color_(DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 1.0F))
    {
    }

    const DocraftColor& DocraftLoomStrokedLine::border_color() const
    {
        return border_color_;
    }

    void DocraftLoomStrokedLine::set_border_color(const DocraftColor& color)
    {
        border_color_ = color;
    }

    float DocraftLoomStrokedLine::border_width() const
    {
        return border_width_;
    }

    void DocraftLoomStrokedLine::set_border_width(float width)
    {
        border_width_ = width;
    }

    DocraftLineStyle DocraftLoomStrokedLine::border_style() const
    {
        return border_style_;
    }

    void DocraftLoomStrokedLine::set_border_style(DocraftLineStyle style)
    {
        border_style_ = style;
    }
} // docraft

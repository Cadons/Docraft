//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/nodes/docraft_loom_rectangle.h"

namespace docraft::loom::nodes {
    void DocraftLoomRectangle::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const DocraftColor& DocraftLoomRectangle::color() const
    {
        return color_;
    }

    void DocraftLoomRectangle::set_color(const DocraftColor& color)
    {
        color_ = color;
    }

    float DocraftLoomRectangle::width() const
    {
        return width_;
    }

    void DocraftLoomRectangle::set_width(float width)
    {
        width_ = width;
    }

    float DocraftLoomRectangle::height() const
    {
        return height_;
    }

    void DocraftLoomRectangle::set_height(float height)
    {
        height_ = height;
    }
} // docraft

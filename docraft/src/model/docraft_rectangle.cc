#include "model/docraft_rectangle.h"

#include <iostream>
#include <ostream>

#include "model/docraft_clone_utils.h"
#include "renderer/docraft_renderer.h"
#include "utils/docraft_logger.h"

namespace docraft::model {
    void DocraftRectangle::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        LOG_DEBUG("Drawing rectangle");
        context->renderer()->render_rectangle(*this);
        draw_children(context);

    }

    void DocraftRectangle::set_background_color(const DocraftColor &color) {
        background_color_ = color;
    }
    void DocraftRectangle::set_border_color(const DocraftColor &color) {
        border_color_ = color;
    }
    void DocraftRectangle::set_border_width(float width) {
        border_width_ = width;
    }

    const DocraftColor & DocraftRectangle::background_color() const {
        return background_color_;
    }
    const DocraftColor & DocraftRectangle::border_color() const {
        return border_color_;
    }
    float DocraftRectangle::border_width() const {
        return border_width_;
    }

    std::shared_ptr<DocraftNode> DocraftRectangle::clone() const {
        auto copy = std::make_shared<DocraftRectangle>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        return copy;
    }
} // docraft

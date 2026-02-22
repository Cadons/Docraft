#include "model/docraft_circle.h"

#include "renderer/docraft_renderer.h"
#include "utils/docraft_logger.h"

namespace docraft::model {
    void DocraftCircle::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        LOG_DEBUG("Drawing circle");
        context->renderer()->render_circle(*this);
    }

    void DocraftCircle::set_background_color(const DocraftColor &color) {
        background_color_ = color;
    }

    void DocraftCircle::set_border_color(const DocraftColor &color) {
        border_color_ = color;
    }

    void DocraftCircle::set_border_width(float width) {
        border_width_ = width;
    }

    const DocraftColor &DocraftCircle::background_color() const {
        return background_color_;
    }

    const DocraftColor &DocraftCircle::border_color() const {
        return border_color_;
    }

    float DocraftCircle::border_width() const {
        return border_width_;
    }

    std::shared_ptr<DocraftNode> DocraftCircle::clone() const {
        return std::make_shared<DocraftCircle>(*this);
    }
} // docraft::model

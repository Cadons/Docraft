#include "docraft/model/docraft_triangle.h"

#include <stdexcept>

#include "docraft/renderer/docraft_renderer.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::model {
    void DocraftTriangle::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        LOG_DEBUG("Drawing triangle");
        context->renderer()->render_triangle(*this);
    }

    void DocraftTriangle::set_points(const std::vector<DocraftPoint> &points) {
        if (points.size() != 3U) {
            throw std::invalid_argument("Triangle requires exactly 3 points");
        }
        points_ = points;
    }

    const std::vector<DocraftPoint> &DocraftTriangle::points() const {
        return points_;
    }

    void DocraftTriangle::set_background_color(const DocraftColor &color) {
        background_color_ = color;
    }

    void DocraftTriangle::set_border_color(const DocraftColor &color) {
        border_color_ = color;
    }

    void DocraftTriangle::set_border_width(float width) {
        border_width_ = width;
    }

    const DocraftColor &DocraftTriangle::background_color() const {
        return background_color_;
    }

    const DocraftColor &DocraftTriangle::border_color() const {
        return border_color_;
    }

    float DocraftTriangle::border_width() const {
        return border_width_;
    }

    std::shared_ptr<DocraftNode> DocraftTriangle::clone() const {
        return std::make_shared<DocraftTriangle>(*this);
    }
} // docraft::model

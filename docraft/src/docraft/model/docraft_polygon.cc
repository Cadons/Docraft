#include "docraft/model/docraft_polygon.h"

#include "docraft/renderer/docraft_renderer.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::model {
    void DocraftPolygon::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        LOG_DEBUG("Drawing polygon");
        context->renderer()->render_polygon(*this);
    }

    void DocraftPolygon::set_points(const std::vector<DocraftPoint> &points) {
        points_ = points;
    }

    const std::vector<DocraftPoint> &DocraftPolygon::points() const {
        return points_;
    }

    void DocraftPolygon::set_background_color(const DocraftColor &color) {
        background_color_ = color;
    }

    void DocraftPolygon::set_border_color(const DocraftColor &color) {
        border_color_ = color;
    }

    void DocraftPolygon::set_border_width(float width) {
        border_width_ = width;
    }

    const DocraftColor &DocraftPolygon::background_color() const {
        return background_color_;
    }

    const DocraftColor &DocraftPolygon::border_color() const {
        return border_color_;
    }

    float DocraftPolygon::border_width() const {
        return border_width_;
    }

    std::shared_ptr<DocraftNode> DocraftPolygon::clone() const {
        return std::make_shared<DocraftPolygon>(*this);
    }
} // docraft::model

#include "docraft/model/docraft_line.h"

#include "docraft/renderer/docraft_renderer.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::model {
    void DocraftLine::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        LOG_DEBUG("Drawing line");
        context->renderer()->render_line(*this);
    }

    void DocraftLine::set_start(const DocraftPoint &point) {
        start_ = point;
    }

    void DocraftLine::set_end(const DocraftPoint &point) {
        end_ = point;
    }

    void DocraftLine::set_border_color(const DocraftColor &color) {
        border_color_ = color;
    }

    void DocraftLine::set_border_width(float width) {
        border_width_ = width;
    }

    const DocraftPoint &DocraftLine::start() const {
        return start_;
    }

    const DocraftPoint &DocraftLine::end() const {
        return end_;
    }

    const DocraftColor &DocraftLine::border_color() const {
        return border_color_;
    }

    float DocraftLine::border_width() const {
        return border_width_;
    }

    std::shared_ptr<DocraftNode> DocraftLine::clone() const {
        return std::make_shared<DocraftLine>(*this);
    }
} // docraft::model

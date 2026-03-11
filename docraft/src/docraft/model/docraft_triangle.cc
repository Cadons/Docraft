/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

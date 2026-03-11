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

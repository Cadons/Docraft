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

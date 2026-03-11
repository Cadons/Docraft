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

#include "docraft/model/docraft_circle.h"

#include "docraft/renderer/docraft_renderer.h"
#include "docraft/utils/docraft_logger.h"

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

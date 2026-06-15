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

#include "docraft/renderer/painter/docraft_triangle_painter.h"

namespace docraft::renderer::painter {
    DocraftTrianglePainter::DocraftTrianglePainter(const model::DocraftTriangle &triangle_node) : triangle_node_(triangle_node) {
    }

    const std::vector<model::DocraftPoint> &DocraftTrianglePainter::shape_points() const {
        return triangle_node_.points();
    }

    const model::DocraftPoint &DocraftTrianglePainter::shape_origin() const {
        return triangle_node_.position();
    }

    const DocraftColor &DocraftTrianglePainter::shape_background_color() const {
        return triangle_node_.background_color();
    }

    const DocraftColor &DocraftTrianglePainter::shape_border_color() const {
        return triangle_node_.border_color();
    }

    float DocraftTrianglePainter::shape_border_width() const {
        return triangle_node_.border_width();
    }
} // docraft::renderer::painter

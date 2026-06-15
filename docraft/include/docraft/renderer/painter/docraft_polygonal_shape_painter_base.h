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

#pragma once

#include <memory>
#include <vector>

#include "docraft/docraft_color.h"
#include "docraft/model/docraft_position.h"
#include "docraft/renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Shared painter base for polygonal shapes (triangle, polygon, ...).
     */
    class DOCRAFT_LIB DocraftPolygonalShapePainterBase : public IPainter {
    public:
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    protected:
        [[nodiscard]] virtual const std::vector<model::DocraftPoint> &shape_points() const = 0;

        [[nodiscard]] virtual const model::DocraftPoint &shape_origin() const = 0;

        [[nodiscard]] virtual const DocraftColor &shape_background_color() const = 0;

        [[nodiscard]] virtual const DocraftColor &shape_border_color() const = 0;

        [[nodiscard]] virtual float shape_border_width() const = 0;
    };
} // namespace docraft::renderer::painter


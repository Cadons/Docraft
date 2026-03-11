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

#include "docraft/docraft_lib.h"
#include "docraft/model/docraft_triangle.h"
#include "docraft/renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftTriangle nodes.
     */
    class DOCRAFT_LIB DocraftTrianglePainter : public IPainter {
    public:
        /**
         * @brief Creates a triangle painter bound to the triangle node.
         * @param triangle_node Triangle node.
         */
        explicit DocraftTrianglePainter(const model::DocraftTriangle &triangle_node);
        /**
         * @brief Draws the triangle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        model::DocraftTriangle triangle_node_;
    };
} // docraft::renderer::painter

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

#include "docraft/backend/pdf/docraft_haru_backend.h"

namespace docraft::backend::pdf {
    /**
     * @brief Haru implementation of line rendering operations.
     */
    class DocraftHaruBackend::LineHaruBackend : public docraft::backend::IDocraftLineRenderingBackend {
    public:
        /**
         * @brief Creates a line backend bound to a Haru document owner.
         */
        explicit LineHaruBackend(DocraftHaruBackend &owner);

        /**
         * @brief Sets the stroke color used for line drawing.
         */
        void set_stroke_color(float r, float g, float b) const override;

        /**
         * @brief Sets the line width used for line drawing.
         */
        void set_line_width(float thickness) const override;

        /**
         * @brief Draws a line segment between two points.
         */
        void draw_line(float x1, float y1, float x2, float y2) const override;

    private:
        DocraftHaruBackend &owner_;
    };
} // namespace docraft::backend::pdf


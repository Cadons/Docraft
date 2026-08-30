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

#include <vector>

#include "docraft/docraft_lib.h"
#include "docraft/docraft_position.h"

namespace docraft::backend {
    /**
     * @brief Interface for line rendering backends used by Docraft.
     */
    class DOCRAFT_LIB IDocraftLineRenderingBackend {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IDocraftLineRenderingBackend() = default;

        /**
         * @brief Sets the stroke color used for subsequent line drawing.
         * @param r Red component in [0,1].
         * @param g Green component in [0,1].
         * @param b Blue component in [0,1].
         */
        virtual void set_stroke_color(float r, float g, float b) const = 0;

        /**
         * @brief Sets the line width used for subsequent line drawing.
         * @param thickness Line width in points.
         */
        virtual void set_line_width(float thickness) const = 0;

        /**
         * @brief Sets the dash pattern used for subsequent line/curve stroking:
         * alternating on/off segment lengths in points, starting with an "on" segment.
         * An empty pattern resets to a solid (continuous) line.
         * @param pattern The on/off segment lengths, in points.
         */
        virtual void set_line_dash_pattern(const std::vector<float>& pattern) const = 0;

        /**
         * @brief Draws a line between two points using the current stroke settings.
         * @param x1 The x-coordinate of the line start.
         * @param y1 The y-coordinate of the line start.
         * @param x2 The x-coordinate of the line end.
         * @param y2 The y-coordinate of the line end.
         */
        virtual void draw_line(float x1, float y1, float x2, float y2) const = 0;

        /**
         * @brief Draws a single smooth curve passing through every point in `points`, in
         * order, using the current stroke settings. Unlike draw_line(), this is not a
         * sequence of straight segments -- the backend is responsible for interpolating
         * a smooth path between consecutive points (e.g. via Catmull-Rom-derived cubic
         * Bezier segments). Requires at least 2 points; a 2-point call degenerates to a
         * straight line.
         * @param points The points the curve passes through, in document coordinates.
         */
        virtual void draw_curve(const std::vector<Position>& points) const = 0;
    };
} // docraft::backend

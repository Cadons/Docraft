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

#include <vector>

#include "docraft/docraft_position.h"

namespace docraft::backend {
    /**
     * @brief Interface for shape rendering backends used by Docraft.
     */
    class DOCRAFT_LIB IDocraftShapeRenderingBackend {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IDocraftShapeRenderingBackend() = default;
        /**
         * @brief Saves the current graphics state.
         */
        virtual void save_state() const = 0;
        /**
         * @brief Restores the previous graphics state.
         */
        virtual void restore_state() const = 0;
        /**
         * @brief Sets the fill color used for subsequent fills.
         * @param r Red component in [0,1].
         * @param g Green component in [0,1].
         * @param b Blue component in [0,1].
         */
        virtual void set_fill_color(float r, float g, float b) const = 0;
        /**
         * @brief Sets the alpha used for subsequent fills.
         * @param alpha Alpha in [0,1].
         */
        virtual void set_fill_alpha(float alpha) const = 0;
        /**
         * @brief Sets the alpha used for subsequent strokes.
         * @param alpha Alpha in [0,1].
         */
        virtual void set_stroke_alpha(float alpha) const = 0;
        /**
         * @brief Defines a rectangle path with the given dimensions.
         * @param x The x-coordinate of the bottom-left corner.
         * @param y The y-coordinate of the bottom-left corner.
         * @param width The rectangle width.
         * @param height The rectangle height.
         */
        virtual void draw_rectangle(float x, float y, float width, float height) const = 0;
        /**
         * @brief Restricts subsequent painting to the given rectangle until the next
         * restore_state(). Callers are responsible for bracketing this with
         * save_state()/restore_state() so the clip doesn't leak past its intended scope.
         * @param x The x-coordinate of the bottom-left corner.
         * @param y The y-coordinate of the bottom-left corner.
         * @param width The rectangle width.
         * @param height The rectangle height.
         */
        virtual void clip_rectangle(float x, float y, float width, float height) const = 0;
        /**
         * @brief Defines a circle path with the given center and radius.
         * @param center_x The x-coordinate of the center.
         * @param center_y The y-coordinate of the center.
         * @param radius The circle radius.
         */
        virtual void draw_circle(float center_x, float center_y, float radius) const = 0;
        /**
         * @brief Defines an ellipse path with the given center and semi-axes.
         *
         * The circle is the `radius_x == radius_y` case; callers may still prefer
         * draw_circle() there, since a backend can have a cheaper dedicated primitive.
         * @param center_x The x-coordinate of the center.
         * @param center_y The y-coordinate of the center.
         * @param radius_x The horizontal semi-axis.
         * @param radius_y The vertical semi-axis.
         */
        virtual void draw_ellipse(float center_x, float center_y, float radius_x, float radius_y) const = 0;
        /**
         * @brief Defines a polygon path with the given points.
         * @param points Polygon points in document coordinates.
         */
        virtual void draw_polygon(const std::vector<Position>& points) const = 0;
        /**
         * @brief Fills the current path.
         */
        virtual void fill() const = 0;
        /**
         * @brief Strokes the current path.
         */
        virtual void stroke() const = 0;
        /**
         * @brief Fills and strokes the current path.
         */
        virtual void fill_stroke() const = 0;
    };
} // docraft::backend

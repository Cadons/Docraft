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

#include "docraft/backend/docraft_shape_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"

#include <memory>

namespace docraft::backend::pdf {
    /**
     * @brief Haru implementation of shape rendering operations.
     *
     * Implements IDocraftShapeRenderingBackend for shape drawing capabilities.
     *
     * Graphics state (fill_alpha, stroke_alpha) is centralized in DocraftHaruSharedState
     * to avoid state distribution across backends.
     *
     * LIFETIME: The shared state must have a registered page operations provider.
     * This is guaranteed if the device is used through DocraftHaruBackend.
     */
    class DocraftHaruShapeBackend : public docraft::backend::IDocraftShapeRenderingBackend {
    public:
        /**
         * @brief Creates a shape backend bound to a Haru document owner.
         *
         * LIFETIME: The shared state must have a registered page operations provider.
         * This is guaranteed if the device is used through DocraftHaruBackend.
         * Uses state's centralized graphics_state for alpha management.
         */
        explicit DocraftHaruShapeBackend(const std::shared_ptr<DocraftHaruSharedState> &state);

        /**
         * @brief Saves the current graphics state.
         */
        void save_state() const override;

        /**
         * @brief Restores the previously saved graphics state.
         */
        void restore_state() const override;

        /**
         * @brief Sets the fill color used for shape painting.
         */
        void set_fill_color(float r, float g, float b) const override;

        /**
         * @brief Sets the fill alpha used for subsequent shape operations.
         */
        void set_fill_alpha(float alpha) const override;

        /**
         * @brief Sets the stroke alpha used for subsequent shape operations.
         */
        void set_stroke_alpha(float alpha) const override;

        /**
         * @brief Appends a rectangle path to the current page path.
         */
        void draw_rectangle(float x, float y, float width, float height) const override;

        /**
         * @brief Restricts subsequent painting to the given rectangle.
         */
        void clip_rectangle(float x, float y, float width, float height) const override;

        /**
         * @brief Appends a circle path to the current page path.
         */
        void draw_circle(float center_x, float center_y, float radius) const override;

        /**
         * @brief Appends a closed polygon path from the provided points.
         */
        void draw_polygon(const std::vector<Position>& points) const override;

        /**
         * @brief Fills the current path.
         */
        void fill() const override;

        /**
         * @brief Strokes the current path.
         */
        void stroke() const override;

        /**
         * @brief Fills and strokes the current path in one operation.
         */
        void fill_stroke() const override;

    private:
        void apply_alpha_state() const;

        std::shared_ptr<DocraftHaruSharedState> state_;
    };
} // namespace docraft::backend::pdf


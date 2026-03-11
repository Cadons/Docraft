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

#include "docraft/docraft_color.h"
#include "docraft/model/docraft_node.h"
#include "docraft/model/docraft_position.h"
#include "docraft/model/i_docraft_clonable.h"

namespace docraft::model {
    /**
     * @brief Triangle node defined by three points.
     */
    class DOCRAFT_LIB DocraftTriangle : public DocraftNode, public IDocraftClonable {
    public:
        using DocraftNode::DocraftNode;
        /**
         * @brief Draws the triangle using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        /**
         * @brief Clones the triangle node.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
        /**
         * @brief Sets the triangle points (local coordinates).
         * @param points Points vector (size must be 3).
         */
        void set_points(const std::vector<DocraftPoint> &points);
        /**
         * @brief Returns the triangle points.
         * @return Points vector.
         */
        const std::vector<DocraftPoint> &points() const;
        /**
         * @brief Sets the background color.
         * @param color Background color.
         */
        void set_background_color(const DocraftColor &color);
        /**
         * @brief Sets the border color.
         * @param color Border color.
         */
        void set_border_color(const DocraftColor &color);
        /**
         * @brief Sets the border width in points.
         * @param width Border width.
         */
        void set_border_width(float width);
        /**
         * @brief Returns the background color.
         * @return Background color.
         */
        const DocraftColor &background_color() const;
        /**
         * @brief Returns the border color.
         * @return Border color.
         */
        const DocraftColor &border_color() const;
        /**
         * @brief Returns the border width in points.
         * @return Border width.
         */
        float border_width() const;
    private:
        std::vector<DocraftPoint> points_{};
        DocraftColor background_color_ = DocraftColor(0, 0, 0, 0);
        DocraftColor border_color_ = DocraftColor(0, 0, 0, 0);
        float border_width_ = 1.0F;
    };
} // docraft::model

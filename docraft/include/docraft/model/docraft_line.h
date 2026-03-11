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

#include "docraft/docraft_color.h"
#include "docraft/model/docraft_node.h"
#include "docraft/model/docraft_position.h"
#include "docraft/model/i_docraft_clonable.h"

namespace docraft::model {
    /**
     * @brief Line node defined by start and end points.
     */
    class DOCRAFT_LIB DocraftLine : public DocraftNode, public IDocraftClonable {
    public:
        using DocraftNode::DocraftNode;
        /**
         * @brief Draws the line using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        /**
         * @brief Clones the line node.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
        /**
         * @brief Sets the line start point (local coordinates).
         * @param point Start point.
         */
        void set_start(const DocraftPoint &point);
        /**
         * @brief Sets the line end point (local coordinates).
         * @param point End point.
         */
        void set_end(const DocraftPoint &point);
        /**
         * @brief Sets the stroke color.
         * @param color Stroke color.
         */
        void set_border_color(const DocraftColor &color);
        /**
         * @brief Sets the stroke width.
         * @param width Stroke width in points.
         */
        void set_border_width(float width);
        /**
         * @brief Returns the start point.
         * @return Start point.
         */
        const DocraftPoint &start() const;
        /**
         * @brief Returns the end point.
         * @return End point.
         */
        const DocraftPoint &end() const;
        /**
         * @brief Returns the stroke color.
         * @return Stroke color.
         */
        const DocraftColor &border_color() const;
        /**
         * @brief Returns the stroke width in points.
         * @return Stroke width.
         */
        float border_width() const;
    private:
        DocraftPoint start_{};
        DocraftPoint end_{};
        DocraftColor border_color_ = DocraftColor(0, 0, 0, 0);
        float border_width_ = 1.0F;
    };
} // docraft::model

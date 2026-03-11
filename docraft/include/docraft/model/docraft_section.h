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

#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_children_container_node.h"

namespace docraft::model {
    /**
     * @brief Rectangular document section with margin configuration.
     *
     * Used as a base for header, body, and footer containers.
     */
    class DOCRAFT_LIB DocraftSection : public DocraftRectangle {
    public:
        using DocraftRectangle::DocraftRectangle;
        DocraftSection();
        DocraftSection(const DocraftSection& node) = default;
        /**
         * @brief Constructs from an existing node pointer (copy).
         */
        explicit DocraftSection(DocraftSection* node);
        /**
         * @brief Draws the section using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        /**
         * @brief Clones the section node and its children.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;

        //getter
        /**
         * @brief Returns the uniform margins value.
         * @return Margin value in points.
         */
        [[nodiscard]] float margins() const;
        /**
         * @brief Returns the top margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_top() const;
        /**
         * @brief Returns the bottom margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_bottom() const;
        /**
         * @brief Returns the left margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_left() const;
        /**
         * @brief Returns the right margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_right() const;
        /**
         * @brief Sets all margins to the same value.
         * @param margins Margin in points.
         */
        void set_margins(float margins);
        /**
         * @brief Sets the top margin.
         * @param margin_top Margin in points.
         */
        void set_margin_top(float margin_top);
        /**
         * @brief Sets the bottom margin.
         * @param margin_bottom Margin in points.
         */
        void set_margin_bottom(float margin_bottom);
        /**
         * @brief Sets the left margin.
         * @param margin_left Margin in points.
         */
        void set_margin_left(float margin_left);
        /**
         * @brief Sets the right margin.
         * @param margin_right Margin in points.
         */
        void set_margin_right(float margin_right);

    private:
        float margins_ = 0.0F;
        float margin_top_ = 0.0F;
        float margin_bottom_ = 0.0F;
        float margin_left_ = 10.0F;
        float margin_right_ = 10.0F;
    };
} // Docraft

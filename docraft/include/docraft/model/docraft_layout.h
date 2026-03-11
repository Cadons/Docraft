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
#include "docraft/model/docraft_children_container_node.h"
#include "docraft/model/i_docraft_clonable.h"
#include "docraft/model/docraft_node.h"

namespace docraft::model {
    /**
     * @brief Layout orientation for child nodes.
     */
    enum class LayoutOrientation {
        kHorizontal,
        kVertical
    };

    /**
     * @brief Layout node that arranges children horizontally or vertically.
     *
     * Children can be weighted to control how available space is distributed.
     */
    class DOCRAFT_LIB DocraftLayout : public DocraftChildrenContainerNode, public IDocraftClonable {
    public:
        using DocraftChildrenContainerNode::DocraftChildrenContainerNode;
        DocraftLayout(const DocraftLayout& node) = default;
        ~DocraftLayout() override = default;
        /**
         * @brief Constructs from an existing node pointer (copy).
         */
        explicit DocraftLayout(DocraftLayout* node);
        /**
         * @brief Returns the layout orientation.
         * @return Orientation enum.
         */
        LayoutOrientation orientation() const;

        /**
         * @brief Sets the layout orientation.
         * @param orientation Orientation enum.
         */
        void set_orientation(LayoutOrientation orientation);
        /**
         * @brief Draws the layout and its children.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        /**
         * @brief Clones the layout node and its children.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
        /**
         * @brief Sets the layout weight for a child at the given index.
         * @param index Child index.
         * @param weight Weight value.
         */
        void set_weight_for_child(int index, float weight) const;

    protected:
        /**
         * @brief Recomputes weights for children.
         */
        void update_weights();

        /**
         * @brief Updates weights when a child is added.
         */
        void on_child_added() override;

        /**
         * @brief Updates weights when a child is removed.
         * @param index Index of removed child.
         */
        void on_child_removed(int index) override;

    public:
        /**
         * @brief Returns the list of child weights.
         * @return Vector of weights.
         */
        std::vector<float> weights() const;

    private:
        LayoutOrientation orientation_;
    };
} // docraft

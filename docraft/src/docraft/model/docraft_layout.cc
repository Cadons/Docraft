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

#include "docraft/model/docraft_layout.h"

#include <iostream>
#include <numeric>
#include <ostream>
#include <utility>

#include "docraft/model/docraft_clone_utils.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::model {
    DocraftLayout::DocraftLayout(DocraftLayout *node) : DocraftChildrenContainerNode(node),
                                                        orientation_(node->orientation_) {
    }

    LayoutOrientation DocraftLayout::orientation() const {
        return orientation_;
    }

    void DocraftLayout::set_orientation(LayoutOrientation orientation) {
        orientation_ = orientation;
    }

    void DocraftLayout::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        LOG_DEBUG("Drawing layout with orientation: " + std::string(orientation_ == LayoutOrientation::kVertical ? "Vertical" : "Horizontal"));
        draw_children(context);
    }

    std::shared_ptr<DocraftNode> DocraftLayout::clone() const {
        auto copy = std::make_shared<DocraftLayout>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        return copy;
    }

     std::vector<float> DocraftLayout::weights() const {
       std::vector<float> copy_weights;
        for (const auto &child: children()) {
            copy_weights.push_back(child->weight());
        }
        return copy_weights;
    }
    void DocraftLayout::on_child_added() {
      update_weights();
    }

    void DocraftLayout::on_child_removed(int  /*index*/) {
        update_weights();
    }

    void DocraftLayout::update_weights() {
        int size = children().size();

    }
    void DocraftLayout::set_weight_for_child(const int index, float weight) const {
        if (index < 0 || std::cmp_greater_equal(index ,children().size())) {
            throw std::out_of_range("Child index out of range");
        }
        children()[index]->set_weight(weight);
    }
} // docraft

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

#include "docraft/model/docraft_foreach.h"

#include "docraft/model/docraft_clone_utils.h"

namespace docraft::model {
    void DocraftForeach::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        //render the children for each item in the collection specified by the "docraft/model" attribute, using template variables for the current item
        draw_children(context);
    }

    std::shared_ptr<DocraftNode> DocraftForeach::clone() const {
        auto copy = std::make_shared<DocraftForeach>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        copy->set_template_nodes(clone_nodes(template_nodes_));
        return copy;
    }

    void DocraftForeach::set_template_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes) {
        template_nodes_ = nodes;
    }

    const std::vector<std::shared_ptr<DocraftNode>>& DocraftForeach::template_nodes() const {
        return template_nodes_;
    }
    void DocraftForeach::set_model(const std::string& model) {
        model_ = model;
    }
    const std::string& DocraftForeach::model() const {
        return model_;
    }

    void DocraftForeach::clear_template_nodes() {
        template_nodes_.clear();
    }
} // docraft

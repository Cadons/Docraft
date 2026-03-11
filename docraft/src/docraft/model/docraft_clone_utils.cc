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

#include "docraft/model/docraft_clone_utils.h"

#include <stdexcept>

namespace docraft::model {
    std::shared_ptr<DocraftNode> clone_node(const std::shared_ptr<DocraftNode> &node) {
        if (!node) {
            return nullptr;
        }
        auto clonable = std::dynamic_pointer_cast<IDocraftClonable>(node);
        if (!clonable) {
            throw std::runtime_error("Node does not implement IDocraftClonable");
        }
        return clonable->clone();
    }

    std::vector<std::shared_ptr<DocraftNode>> clone_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes) {
        std::vector<std::shared_ptr<DocraftNode>> result;
        result.reserve(nodes.size());
        for (const auto &node : nodes) {
            result.emplace_back(clone_node(node));
        }
        return result;
    }
} // namespace docraft::model

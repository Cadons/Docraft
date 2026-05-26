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

#include "docraft_document_query.h"
#include "docraft/model/docraft_node.h"

namespace docraft::management {
    template<typename T>
    std::vector<std::shared_ptr<const T> > DocraftDocumentQuery::find_by_type(
        const std::vector<std::shared_ptr<model::DocraftNode> > &root) {
        std::vector<std::shared_ptr<const T> > result;
        traverse_dom(root, [&](const std::shared_ptr<model::DocraftNode> &node, DocraftDomTraverseOp op) {
            if (op != DocraftDomTraverseOp::kEnter) {
                return;
            }
            if (auto casted = std::dynamic_pointer_cast<const T>(node)) {
                result.push_back(casted);
            }
        });
        return result;
    }

    template<typename T>
    std::vector<std::shared_ptr<T> > DocraftDocumentQuery::take_by_type(
        std::vector<std::shared_ptr<model::DocraftNode> > &root) {
        std::vector<std::shared_ptr<T> > result;
        traverse_dom(root, [&](const std::shared_ptr<model::DocraftNode> &node, DocraftDomTraverseOp op) {
            if (op != DocraftDomTraverseOp::kEnter) {
                return;
            }
            if (auto casted = std::dynamic_pointer_cast<T>(node)) {
                result.push_back(casted);
            }
        });
        return result;
    }
}


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

#include "docraft/management/docraft_document_query.h"
#include "docraft/model/docraft_node.h"
#include "docraft/model/docraft_children_container_node.h"

namespace docraft::management {
    std::vector<std::shared_ptr<const model::DocraftNode> > DocraftDocumentQuery::find_by_name(
        const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name) {
        std::vector<std::shared_ptr<const model::DocraftNode> > result;
        for (const auto &node: find_by_name_impl(root, name)) {
            result.push_back(node);
        }
        return result;
    }

    std::vector<std::shared_ptr<model::DocraftNode> > DocraftDocumentQuery::take_by_name(
        std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name) {
        return find_by_name_impl(root, name);
    }

    std::vector<std::shared_ptr<model::DocraftNode> > DocraftDocumentQuery::find_by_name_impl(
        const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name) {
        std::vector<std::shared_ptr<model::DocraftNode> > result;
        traverse_dom(root, [&](const std::shared_ptr<model::DocraftNode> &node, DocraftDomTraverseOp op) {
            if (op != DocraftDomTraverseOp::kEnter) {
                return;
            }
            if (node && node->node_name() == name) {
                result.push_back(node);
            }
        });
        return result;
    }

    std::shared_ptr<const model::DocraftNode> DocraftDocumentQuery::find_first_by_name(
        const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name) {
        const auto matches = find_by_name(root, name);
        return matches.empty() ? nullptr : matches.front();
    }

    std::shared_ptr<model::DocraftNode> DocraftDocumentQuery::take_first_by_name(
        std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name) {
        const auto matches = take_by_name(root, name);
        return matches.empty() ? nullptr : matches.front();
    }

    std::shared_ptr<const model::DocraftNode> DocraftDocumentQuery::find_last_by_name(
        const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name) {
        const auto matches = find_by_name(root, name);
        return matches.empty() ? nullptr : matches.back();
    }

    std::shared_ptr<model::DocraftNode> DocraftDocumentQuery::take_last_by_name(
        std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name) {
        const auto matches = take_by_name(root, name);
        return matches.empty() ? nullptr : matches.back();
    }

    void DocraftDocumentQuery::traverse_dom(
        const std::vector<std::shared_ptr<model::DocraftNode> > &root,
        const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback) {
        for (const auto &node: root) {
            traverse_node(node, callback);
        }
    }

    void DocraftDocumentQuery::traverse_node(
        const std::shared_ptr<model::DocraftNode> &node,
        const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback) {
        if (!node) {
            return;
        }
        callback(node, DocraftDomTraverseOp::kEnter);
        if (auto parent_node = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            for (const auto &child_node: parent_node->children()) {
                traverse_node(child_node, callback);
            }
        }
        callback(node, DocraftDomTraverseOp::kExit);
    }
} // docraft::management


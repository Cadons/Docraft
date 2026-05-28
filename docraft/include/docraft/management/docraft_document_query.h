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
#include <memory>
#include <functional>
#include <string>

namespace docraft::model {
    class DocraftNode;
}

namespace docraft::management {
    enum class DocraftDomTraverseOp {
        kEnter,
        kExit
    };

    /**
     * @brief Provides query and traversal operations on document DOM trees.
     *
     * Separate responsibility for finding nodes and traversing the DOM.
     */
    class DOCRAFT_LIB DocraftDocumentQuery {
    public:
        /**
         * @brief Finds nodes by name in a DOM tree.
         * @param root The root nodes to start searching from.
         * @param name Node name to search for.
         * @return Vector of nodes matching the name, or empty vector if none found.
         */
        static std::vector<std::shared_ptr<const model::DocraftNode> > find_by_name(
            const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name);

        static std::vector<std::shared_ptr<model::DocraftNode> > take_by_name(
            const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name);

        /**
         * @brief Finds the first node by name in a DOM tree.
         * @param root The root nodes to start searching from.
         * @param name Node name to search for.
         * @return Shared pointer to the first matching node, or nullptr if not found.
         */
        static std::shared_ptr<const model::DocraftNode> find_first_by_name(
            const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name);

        static std::shared_ptr<model::DocraftNode> take_first_by_name(
            std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name);

        /**
         * @brief Finds the last node by name in a DOM tree.
         * @param root The root nodes to start searching from.
         * @param name Node name to search for.
         * @return Shared pointer to the last matching node, or nullptr if not found.
         */
        static std::shared_ptr<const model::DocraftNode> find_last_by_name(
            const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name);

        static std::shared_ptr<model::DocraftNode> take_last_by_name(
            std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name);

        /**
         * @brief Finds nodes by type in a DOM tree.
         * @tparam T Node type to search for.
         * @param root The root nodes to start searching from.
         * @return Vector of nodes matching the type, or empty vector if none found.
         */
        template<typename T>
        static std::vector<std::shared_ptr<const T> > find_by_type(
            const std::vector<std::shared_ptr<model::DocraftNode> > &root);

        template<typename T>
        static std::vector<std::shared_ptr<T> > take_by_type(
            std::vector<std::shared_ptr<model::DocraftNode> > &root);

        /**
         * @brief Traverses a DOM tree and executes a callback on each node.
         * @param root The root nodes to traverse.
         * @param callback Function called for each node and operation (enter/exit).
         */
        static void traverse_dom(
            const std::vector<std::shared_ptr<model::DocraftNode> > &root,
            const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback);

    private:
        static void traverse_node(
            const std::shared_ptr<model::DocraftNode> &node,
            const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback);

        static std::vector<std::shared_ptr<model::DocraftNode> > find_by_name_impl(
            const std::vector<std::shared_ptr<model::DocraftNode> > &root, const std::string &name);
    };
}

#include "docraft_document_query.hpp"


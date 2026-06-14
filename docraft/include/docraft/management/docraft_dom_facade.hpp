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

namespace docraft::management {
    template<typename Derived>
    std::vector<std::shared_ptr<const model::DocraftNode> >
    DocraftDOMFacade<Derived>::find_by_name(const std::string &name) const {
        std::vector<std::shared_ptr<const model::DocraftNode> > result;
        self().traverse_dom([&result, &name](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (op != decltype(op)::kEnter || !node) {
                return;
            }
            if (node->node_name() == name) {
                result.push_back(node);
            }
        });
        return result;
    }

    template<typename Derived>
    std::vector<std::shared_ptr<model::DocraftNode> >
    DocraftDOMFacade<Derived>::take_by_name(const std::string &name) const {
        std::vector<std::shared_ptr<model::DocraftNode> > result;
        self().traverse_dom([&result, &name](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (op != decltype(op)::kEnter || !node) {
                return;
            }
            if (node->node_name() == name) {
                result.push_back(node);
            }
        });
        return result;
    }

    template<typename Derived>
    std::shared_ptr<const model::DocraftNode>
    DocraftDOMFacade<Derived>::find_first_by_name(const std::string &name) const {
        std::shared_ptr<const model::DocraftNode> result;
        self().traverse_dom([&result, &name](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (result || op != decltype(op)::kEnter || !node) {
                return;
            }
            if (node->node_name() == name) {
                result = node;
            }
        });
        return result;
    }

    template<typename Derived>
    std::shared_ptr<model::DocraftNode>
    DocraftDOMFacade<Derived>::take_first_by_name(const std::string &name) {
        std::shared_ptr<model::DocraftNode> result;
        self().traverse_dom([&result, &name](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (result || op != decltype(op)::kEnter || !node) {
                return;
            }
            if (node->node_name() == name) {
                result = node;
            }
        });
        return result;
    }

    template<typename Derived>
    std::shared_ptr<const model::DocraftNode>
    DocraftDOMFacade<Derived>::find_last_by_name(const std::string &name) const {
        std::shared_ptr<const model::DocraftNode> result;
        self().traverse_dom([&result, &name](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (op != decltype(op)::kEnter || !node) {
                return;
            }
            if (node->node_name() == name) {
                result = node;
            }
        });
        return result;
    }

    template<typename Derived>
    std::shared_ptr<model::DocraftNode>
    DocraftDOMFacade<Derived>::take_last_by_name(const std::string &name) {
        std::shared_ptr<model::DocraftNode> result;
        self().traverse_dom([&result, &name](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (op != decltype(op)::kEnter || !node) {
                return;
            }
            if (node->node_name() == name) {
                result = node;
            }
        });
        return result;
    }

    template<typename Derived>
    template<typename T>
    std::vector<std::shared_ptr<const T> > DocraftDOMFacade<Derived>::find_by_type() const {
        std::vector<std::shared_ptr<const T> > result;
        self().traverse_dom([&result](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (op != decltype(op)::kEnter) {
                return;
            }
            if (auto casted = std::dynamic_pointer_cast<const T>(node)) {
                result.push_back(casted);
            }
        });
        return result;
    }

    template<typename Derived>
    template<typename T>
    std::vector<std::shared_ptr<T> > DocraftDOMFacade<Derived>::take_by_type() {
        std::vector<std::shared_ptr<T> > result;
        self().traverse_dom([&result](const std::shared_ptr<model::DocraftNode> &node, auto op) {
            if (op != decltype(op)::kEnter) {
                return;
            }
            if (auto casted = std::dynamic_pointer_cast<T>(node)) {
                result.push_back(casted);
            }
        });
        return result;
    }

    template<typename Derived>
    Derived &DocraftDOMFacade<Derived>::self() {
        return static_cast<Derived &>(*this);
    }

    template<typename Derived>
    const Derived &DocraftDOMFacade<Derived>::self() const {
        return static_cast<const Derived &>(*this);
    }
} // namespace docraft::management


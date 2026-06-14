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

#include <memory>
#include <string>
#include <vector>

#include "docraft/management/docraft_document_config.h"
#include "docraft/model/docraft_node.h"

namespace docraft::management {
    /**
     * @brief CRTP facade exposing DOM query shortcuts for DocraftDocument-like classes.
     */
    template<typename Derived>
    class DocraftDOMFacade {
    public:
        [[nodiscard]] std::vector<std::shared_ptr<const model::DocraftNode> > find_by_name(
            const std::string &name) const;

        [[nodiscard]] std::vector<std::shared_ptr<model::DocraftNode> > take_by_name(const std::string &name) const;

        [[nodiscard]] std::shared_ptr<const model::DocraftNode> find_first_by_name(const std::string &name) const;

        [[nodiscard]] std::shared_ptr<model::DocraftNode> take_first_by_name(const std::string &name);

        [[nodiscard]] std::shared_ptr<const model::DocraftNode> find_last_by_name(const std::string &name) const;

        [[nodiscard]] std::shared_ptr<model::DocraftNode> take_last_by_name(const std::string &name);

        template<typename T>
        [[nodiscard]] std::vector<std::shared_ptr<const T> > find_by_type() const;

        template<typename T>
        [[nodiscard]] std::vector<std::shared_ptr<T> > take_by_type();

    private:
        [[nodiscard]] Derived &self();

        [[nodiscard]] const Derived &self() const;
    };
} // namespace docraft::management

#include "docraft_dom_facade.hpp"

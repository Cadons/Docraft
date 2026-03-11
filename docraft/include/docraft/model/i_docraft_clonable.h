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

#include <memory>

namespace docraft::model {
    class DocraftNode;

    /**
     * @brief Interface for clonable Docraft nodes.
     * Nodes that implement this interface can be cloned, which is useful for templating and other operations that require copying nodes.
     */
    class DOCRAFT_LIB IDocraftClonable {
    public:
        virtual ~IDocraftClonable() = default;
        /**
         * @brief Creates a deep copy of the node.
         * @return A shared pointer to the cloned node.
         */
        virtual std::shared_ptr<DocraftNode> clone() const = 0;
    };
} // namespace docraft::model

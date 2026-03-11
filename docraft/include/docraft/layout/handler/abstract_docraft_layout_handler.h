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

#include "docraft/docraft_document_context.h"
#include "docraft/model/docraft_node.h"
#include "docraft/generic/chain_of_responsibility_handler.h"

namespace docraft::layout::handler {
    template<typename T>
    requires std::derived_from<T, model::DocraftNode>
    /**
     * @brief Base class for layout handlers in the chain of responsibility.
     *
     * Each handler is responsible for computing layout boxes for a specific
     * node type and is given access to the shared document context.
     *
     * @tparam T Node type handled by this handler.
     */
    class AbstractDocraftLayoutHandler :public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode,model::DocraftTransform> {
    public:
        using DocraftChainOfResponsibilityHandler::DocraftChainOfResponsibilityHandler;
        ~AbstractDocraftLayoutHandler() override = default;

        /**
         * @brief Creates a handler bound to a document context.
         * @throws std::invalid_argument if context is null.
         */
        explicit AbstractDocraftLayoutHandler(const std::shared_ptr<DocraftDocumentContext> &context) : context_(context) {
            if (!context_) {
                throw std::invalid_argument("context is null");
            }
        }
        /**
         * @brief Computes the layout box for a concrete node type.
         * @param node Node to layout.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        virtual void compute(const std::shared_ptr<T>& node, model::DocraftTransform* box, DocraftCursor& cursor) =0;
        /**
         * @brief Returns the bound document context.
         * @return Document context.
         */
        std::shared_ptr<DocraftDocumentContext> context() const {
            return context_;
        }
    protected:
        std::shared_ptr<DocraftDocumentContext> context_;
    };
}

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

// C++
#pragma once

#include "docraft/docraft_lib.h"
#include <memory>
#include <vector>

#include "docraft/docraft_cursor.h"
#include "docraft/model/docraft_node.h"

namespace docraft {
    class DocraftDocumentContext;
}

namespace docraft::layout {
    /**
     * @brief Computes layout boxes for document nodes using a chain of handlers.
     *
     * The engine walks node trees and delegates box computation to specialized
     * handlers (text, table, list, etc.), maintaining a cursor for flow layout.
     */
    class DOCRAFT_LIB DocraftLayoutEngine {
    public:
        /**
         * @brief Creates a layout engine for the given document context.
         * @param context Document context used for measurements and page info.
         * @param reset_cursor Whether to reset the cursor before layout.
         */
        explicit DocraftLayoutEngine(const std::shared_ptr<DocraftDocumentContext>& context, bool reset_cursor = true);
        DocraftLayoutEngine(const DocraftLayoutEngine&) = delete;
        DocraftLayoutEngine& operator=(const DocraftLayoutEngine&) = delete;

        DocraftLayoutEngine(DocraftLayoutEngine &&) noexcept;

        DocraftLayoutEngine &operator=(DocraftLayoutEngine &&) noexcept;

        /**
         * @brief Destructor.
         */
        virtual ~DocraftLayoutEngine();

        /**
         * @brief Computes the layout for a single node tree.
         * @param node A shared pointer to a DocraftNode object.
         * @return A DocraftTransform representing the computed layout box.
         */
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode>& node);
        /**
         * @brief Computes the layout for a single node tree with a custom cursor.
         * @param node A shared pointer to a DocraftNode object.
         * @param cursor Cursor used for layout traversal.
         * @return A DocraftTransform representing the computed layout box.
         */
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode>& node, DocraftCursor& cursor);
        /**
         * @brief Computes the layout for a full document represented by a vector of nodes.
         * @param nodes A vector of shared pointers to DocraftNode objects.
         */
        void compute_document_layout(const std::vector<std::shared_ptr<model::DocraftNode>>& nodes);
    protected:
        /**
         * @brief Returns the bound document context.
         * @return Document context.
         */
        const std::shared_ptr<DocraftDocumentContext>& context() const;
        /**
         * @brief Computes the maximum rectangle that can contain all the given boxes.
         * @param boxes A vector of DocraftTransform representing the boxes.
         * @return A DocraftTransform representing the maximum rectangle.
         */
        static model::DocraftTransform compute_max_rect(const std::vector<model::DocraftTransform>& boxes) ;

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };
} // layout

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

#include "docraft/model/docraft_text.h"

namespace docraft::model {
    /**
     * @brief Text node that renders the current page number.
     *
     * The value is managed by the library and resolved from the document context
     * at layout/render time.
     */
    class DOCRAFT_LIB DocraftPageNumber : public DocraftText {
    public:
        using DocraftText::DocraftText;
        DocraftPageNumber();
        ~DocraftPageNumber() override = default;

        /**
         * @brief Updates the internal text to the current page number.
         * @param context Document context.
         */
        void update_text_from_context(const std::shared_ptr<DocraftDocumentContext>& context);
        /**
         * @brief Draws the page number using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext>& context) override;
        /**
         * @brief Clones the page number node and its line children.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
    };
} // docraft::model

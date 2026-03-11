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
#include "abstract_docraft_layout_handler.h"
#include "docraft/generic/chain_of_responsibility_handler.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_text.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for text nodes.
     *
     * Measures text width, handles alignment, and manages line breaks.
     */
    class DOCRAFT_LIB DocraftLayoutTextHandler : public AbstractDocraftLayoutHandler<model::DocraftText> {
    public:

        /**
         * @brief Computes the layout box for a text node.
         * @param node Text node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftText>& node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        using AbstractDocraftLayoutHandler::AbstractDocraftLayoutHandler;

        /**
         * @brief Handles a node if it is a DocraftText.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;

    protected:
        /**
         * @brief Filters text content before layout (e.g., trims whitespace).
         * @param node Text node to filter.
         */
        static void filter_text(const std::shared_ptr<model::DocraftText>& node);
        /**
         * @brief Measures width of a text node using the backend.
         * @param node Text node.
         * @return Width in points.
         */
        float measure_text_width(const std::shared_ptr<model::DocraftText>& node) const;
        /**
         * @brief Measures width of a raw text string using the backend.
         * @param text Text string.
         * @return Width in points.
         */
        float measure_test_width(const std::string& text) const;
    private:
        const float interline_space_ = 1.2F;

    };
} // docraft

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
#include "docraft/renderer/painter/i_painter.h"
#include "docraft/model/docraft_blank_line.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that advances layout for blank lines.
     *
     * Performs no drawing but updates the cursor state.
     */
    class DOCRAFT_LIB docraft_blank_line_painter : public renderer::painter::IPainter {
    public:
        /**
         * @brief Creates a blank-line painter bound to the blank line node.
         * @param blank_line_node Blank line node.
         */
        explicit docraft_blank_line_painter(const model::DocraftBlankLine &blank_line_node);

        /**
         * @brief Draws the blank line using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    private:
        const model::DocraftBlankLine &blank_line_node_;
    };
} // docraft

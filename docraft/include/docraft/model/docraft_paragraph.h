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
#include "docraft/model/docraft_section.h"
#include "docraft/model/i_docraft_clonable.h"

namespace docraft::model {
        /**
         * @brief Paragraph node (placeholder for future behavior).
         *
         * Currently inherits DocraftNode without additional data or rendering logic.
         */
        class DOCRAFT_LIB DocraftParagraph : public DocraftNode, public IDocraftClonable {
        public:
            /**
             * @brief Draws the paragraph using the provided context.
             */
            void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
            /**
             * @brief Clones the paragraph node.
             * @return Shared pointer to the cloned node.
             */
            std::shared_ptr<DocraftNode> clone() const override;
        };
} // docraft

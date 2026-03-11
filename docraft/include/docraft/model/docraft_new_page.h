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

#include "docraft/model/docraft_node.h"
#include "i_docraft_clonable.h"

namespace docraft::model {
    /**
     * @brief Represents a manual page break in the document.
     */
    class DOCRAFT_LIB DocraftNewPage : public DocraftNode, public IDocraftClonable {
    public:
        using DocraftNode::DocraftNode;
        DocraftNewPage() = default;
        ~DocraftNewPage() override = default;

        void draw(const std::shared_ptr<DocraftDocumentContext> &/*context*/) override {
            // Layout engine handles page breaks; no rendering needed.
        }

        std::shared_ptr<DocraftNode> clone() const override {
            return std::make_shared<DocraftNewPage>(*this);
        }
    };
} // docraft

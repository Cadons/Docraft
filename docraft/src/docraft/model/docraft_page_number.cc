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

#include "docraft/model/docraft_page_number.h"

#include <string>
#include <stdexcept>

#include "docraft/backend/docraft_page_rendering_backend.h"
#include "docraft/model/docraft_clone_utils.h"
#include "docraft/renderer/docraft_renderer.h"

namespace docraft::model {
    DocraftPageNumber::DocraftPageNumber() {
        set_text("xxxxx"); // Placeholder text to reserve space during layout until the actual page number is resolved.
    }

    void DocraftPageNumber::update_text_from_context(const std::shared_ptr<DocraftDocumentContext>& context) {
        std::size_t page_number = 1;
        if (context) {
            if (const auto& page_backend = context->page_backend()) {
                page_number = page_backend->current_page_number();
            }
        }
        for (const auto& line : lines()) {
            line->set_text(std::to_string(page_number));
        }
    }

    void DocraftPageNumber::draw(const std::shared_ptr<DocraftDocumentContext>& context) {
        update_text_from_context(context);
        context->renderer()->render_text(*this);
    }

    std::shared_ptr<DocraftNode> DocraftPageNumber::clone() const {
        auto copy = std::make_shared<DocraftPageNumber>(*this);
        copy->clear_lines();
        for (const auto &child : children()) {
            auto cloned_child = clone_node(child);
            auto text_child = std::dynamic_pointer_cast<DocraftText>(cloned_child);
            if (!text_child) {
                throw std::runtime_error("Page number line child does not clone to DocraftText");
            }
            copy->add_line(text_child);
        }
        return copy;
    }
} // docraft::model

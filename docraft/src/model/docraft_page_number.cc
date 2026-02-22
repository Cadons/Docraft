#include "model/docraft_page_number.h"

#include <string>
#include <stdexcept>

#include "backend/docraft_page_rendering_backend.h"
#include "model/docraft_clone_utils.h"
#include "renderer/docraft_renderer.h"

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

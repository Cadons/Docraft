#include "model/docraft_page_number.h"

#include <string>

#include "backend/docraft_page_rendering_backend.h"
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
} // docraft::model

//
// Created by Matteo on 29/12/25.
//

#include "model/docraft_paragraph.h"

namespace docraft::model {
    void DocraftParagraph::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        (void)context;
    }

    std::shared_ptr<DocraftNode> DocraftParagraph::clone() const {
        return std::make_shared<DocraftParagraph>(*this);
    }
} // namespace docraft::model

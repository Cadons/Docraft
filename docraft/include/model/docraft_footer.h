#pragma once

#include "docraft_lib.h"
#include <memory>

#include "docraft_document_context.h"
#include "model/docraft_section.h"

namespace docraft::model {
    /**
     * @brief Footer section of the document.
     */
    class DOCRAFT_LIB DocraftFooter : public DocraftSection {
    public:
        /**
         * @brief Creates a footer section with default margins.
         */
        DocraftFooter();
        ~DocraftFooter() override = default;
        /**
         * @brief Clones the footer node and its children.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
    };
} // docraft

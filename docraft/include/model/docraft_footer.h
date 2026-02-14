#pragma once
#include <memory>

#include "docraft_document_context.h"
#include "model/docraft_section.h"

namespace docraft::model {
    /**
     * @brief Footer section of the document.
     */
    class DocraftFooter : public DocraftSection {
    public:
        /**
         * @brief Creates a footer section with default margins.
         */
        DocraftFooter();
        ~DocraftFooter() override = default;
    };
} // docraft

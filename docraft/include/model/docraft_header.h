#pragma once
#include "docraft_section.h"

namespace docraft::model {
    /**
     * @brief Header section of the document.
     */
    class DocraftHeader : public DocraftSection {
    public:
        /**
         * @brief Creates a header section with default margins.
         */
        DocraftHeader();
        ~DocraftHeader() override = default;
    };
}

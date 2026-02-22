#pragma once

#include "docraft_lib.h"
#include "docraft_section.h"

namespace docraft::model {
    /**
     * @brief Header section of the document.
     */
    class DOCRAFT_LIB DocraftHeader : public DocraftSection {
    public:
        /**
         * @brief Creates a header section with default margins.
         */
        DocraftHeader();
        ~DocraftHeader() override = default;
        /**
         * @brief Clones the header node and its children.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
    };
}

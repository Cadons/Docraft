#pragma once

#include "docraft_lib.h"
#include "model/docraft_section.h"

namespace docraft::model {
    /**
     * @brief Body section of the document.
     */
    class DOCRAFT_LIB DocraftBody : public DocraftSection{
        public:
        using DocraftSection::DocraftSection;
        ~DocraftBody() override = default;
        /**
         * @brief Clones the body node and its children.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
    };
} // docraft

#pragma once
#include "model/docraft_section.h"

namespace docraft::model {
    /**
     * @brief Body section of the document.
     */
    class DocraftBody : public DocraftSection{
        public:
        using DocraftSection::DocraftSection;
        ~DocraftBody() override = default;
    };
} // docraft

#pragma once
#include "model/docraft_section.h"

namespace docraft::model {
    class DocraftBody : public DocraftSection{
        public:
        using DocraftSection::DocraftSection;
        ~DocraftBody() override = default;
    };
} // docraft

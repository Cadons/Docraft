#pragma once
#include "docraft_section.h"

namespace docraft::model {
    class DocraftHeader : public DocraftSection {
    public:
        DocraftHeader();
        ~DocraftHeader() override = default;
    };
}

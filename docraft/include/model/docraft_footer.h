#pragma once
#include <memory>

#include "docraft_document_context.h"
#include "model/docraft_section.h"

namespace docraft::model {
    class DocraftFooter : public DocraftSection {
    public:
        DocraftFooter();
        ~DocraftFooter() override = default;
    };
} // docraft

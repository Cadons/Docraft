#pragma once
#include <memory>

#include "docraft_pdf_context.h"

namespace docraft::renderer::painter {
    /**
     * @brief Interface for painter classes responsible for drawing elements onto a PDF context.
     */
    class IPainter {
        public:
        virtual ~IPainter() = default;
        virtual void draw(const std::shared_ptr<DocraftPDFContext> &context) = 0;
    };
} // docraft

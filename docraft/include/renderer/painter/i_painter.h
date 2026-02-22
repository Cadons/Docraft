#pragma once

#include "docraft_lib.h"
#include <memory>

#include "docraft_document_context.h"

namespace docraft::renderer::painter {
    /**
     * @brief Interface for painter classes responsible for drawing elements onto a PDF context.
     */
    class DOCRAFT_LIB IPainter {
        public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPainter() = default;
        /**
         * @brief Draws the element using the provided document context.
         * @param context Document context providing backend access and state.
         */
        virtual void draw(const std::shared_ptr<DocraftDocumentContext> &context) = 0;
    };
} // docraft

#pragma once

#include "docraft_lib.h"
#include "docraft_section.h"
#include "model/i_docraft_clonable.h"

namespace docraft::model {
        /**
         * @brief Paragraph node (placeholder for future behavior).
         *
         * Currently inherits DocraftNode without additional data or rendering logic.
         */
        class DOCRAFT_LIB DocraftParagraph : public DocraftNode, public IDocraftClonable {
        public:
            /**
             * @brief Draws the paragraph using the provided context.
             */
            void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
            /**
             * @brief Clones the paragraph node.
             * @return Shared pointer to the cloned node.
             */
            std::shared_ptr<DocraftNode> clone() const override;
        };
} // docraft

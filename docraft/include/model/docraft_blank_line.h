#pragma once
#include "docraft_node.h"
#include "model/i_docraft_clonable.h"

namespace docraft::model {
        /**
         * @brief Blank line node used to add vertical spacing in flow layout.
         */
        class DocraftBlankLine : public DocraftNode, public IDocraftClonable {
        public:
            using DocraftNode::DocraftNode;
            /**
             * @brief Draws the blank line using the provided context.
             */
            void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
            /**
             * @brief Clones the blank line node.
             * @return Shared pointer to the cloned node.
             */
            std::shared_ptr<DocraftNode> clone() const override;
        };
} // docraft

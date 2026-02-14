#pragma once
#include "docraft_node.h"

namespace docraft::model {
        /**
         * @brief Blank line node used to add vertical spacing in flow layout.
         */
        class DocraftBlankLine : public DocraftNode {
        public:
            using DocraftNode::DocraftNode;
            /**
             * @brief Draws the blank line using the provided context.
             */
            void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
        };
} // docraft

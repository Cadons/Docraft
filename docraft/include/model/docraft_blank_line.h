#pragma once
#include "docraft_node.h"

namespace docraft::model {
        class DocraftBlankLine : public DocraftNode {
        public:
            using DocraftNode::DocraftNode;
            void draw(const std::shared_ptr<DocraftPDFContext> &context) override;
        };
} // docraft

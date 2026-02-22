#pragma once

#include "docraft_lib.h"

#include "docraft_node.h"
#include "model/i_docraft_clonable.h"

namespace docraft::model {
    /**
     * @brief Represents a manual page break in the document.
     */
    class DOCRAFT_LIB DocraftNewPage : public DocraftNode, public IDocraftClonable {
    public:
        using DocraftNode::DocraftNode;
        DocraftNewPage() = default;
        ~DocraftNewPage() override = default;

        void draw(const std::shared_ptr<DocraftDocumentContext> &/*context*/) override {
            // Layout engine handles page breaks; no rendering needed.
        }

        std::shared_ptr<DocraftNode> clone() const override {
            return std::make_shared<DocraftNewPage>(*this);
        }
    };
} // docraft

#pragma once
#include <vector>
#include <memory>

#include "docraft_pdf_context.h"
#include "model/docraft_node.h"
namespace docraft {
    class DocraftDocument {
        public:
        DocraftDocument(std::string document_title="Untitled Document");
        virtual ~DocraftDocument()=default;
        void add_node(const std::shared_ptr<model::DocraftNode>& node);
        void render();
        void set_document_title(const std::string &document_title);
        std::string document_title();
    private:
        std::shared_ptr<DocraftPDFContext> pdf_context_;
        std::string document_title_;
        std::vector<std::shared_ptr<model::DocraftNode>> dom_;
    };
}

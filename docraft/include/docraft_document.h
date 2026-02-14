#pragma once
#include <vector>
#include <memory>

#include "docraft_document_context.h"
#include "model/docraft_node.h"
#include "model/docraft_settings.h"

namespace docraft {
    class DocraftDocument {
        public:
        DocraftDocument(std::string document_title="Untitled Document");
        virtual ~DocraftDocument()=default;
        void add_node(const std::shared_ptr<model::DocraftNode>& node);

        void configure_document_settings() const;

        void render();
        void set_document_title(const std::string &document_title);
        std::string document_title();
        void set_settings(const std::shared_ptr<model::DocraftSettings> &settings);
        std::shared_ptr<model::DocraftSettings> settings() const;
    private:
        std::shared_ptr<DocraftDocumentContext> pdf_context_;
        std::shared_ptr<model::DocraftSettings> settings_;
        std::string document_title_;
        std::vector<std::shared_ptr<model::DocraftNode>> dom_;
    };
}

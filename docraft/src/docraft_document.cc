//
// Created by Matteo on 28/12/25.
//

#include "docraft_document.h"

#include <hpdf.h>
#include <iostream>
#include <ostream>
#include <list>
#include <sstream>

#include "layout/docraft_layout_engine.h"
#include "layout/handler/docraft_layout_handler.h"
#include "renderer/docraft_pdf_renderer.h"

namespace docraft {
    DocraftDocument::DocraftDocument(std::string document_title) : document_title_(std::move(document_title)) {
    }

    void DocraftDocument::add_node(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node) {
            throw std::runtime_error("Null node cannot be added to the document");
        }
        dom_.emplace_back(node);
    }
    void DocraftDocument::render() {
        pdf_context_ = std::make_shared<DocraftDocumentContext>();
        pdf_context_->set_renderer(std::make_shared<renderer::DocraftPDFRenderer>(pdf_context_));
        pdf_context_->set_font_applier(std::make_shared<generic::DocraftFontApplier>(pdf_context_));
        std::cout << "render" << std::endl;
        // Layout phase
        layout::DocraftLayoutEngine layout_engine(pdf_context_);
        layout_engine.compute_document_layout(dom_);

        // Rendering phase
        for (auto &node : dom_) {
            if (node) {
                node->draw(pdf_context_);
            }
        }

        HPDF_SaveToFile(pdf_context_->pdf_doc(), (document_title_ + ".pdf").c_str());
    }
    void DocraftDocument::set_document_title(const std::string &document_title) {
        document_title_ = document_title;
    }
    std::string DocraftDocument::document_title() {
        return document_title_;
    }
} // docraft
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
#include "layout/handler/docraft_layout_section_handler.h"
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
        pdf_context_ = std::make_shared<DocraftPDFContext>();
        pdf_context_->set_renderer(std::make_shared<renderer::DocraftPDFRenderer>(pdf_context_));
        pdf_context_->set_font_applier(std::make_shared<generic::DocraftFontApplier>(pdf_context_));
        std::cout << "render" << std::endl;
        // Layout phase
        for (auto &node : dom_) {
            if (node) {
                docraft::layout::DocraftLayoutEngine::layout(node, pdf_context_);
            }
        }
        // Post-processing phase, this phase is needed to adjust sections after layouting
        layout::handler::DocraftLayoutSectionHandler section_handler(pdf_context_);
        for (auto &node : dom_) {
            if (auto section_node = std::dynamic_pointer_cast<model::DocraftSection>(node)) {
                section_handler.post_process_layouts(section_node);
            }
        }
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
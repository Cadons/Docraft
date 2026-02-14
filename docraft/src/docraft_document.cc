//
// Created by Matteo on 28/12/25.
//

#include "docraft_document.h"

#include <iostream>
#include <ostream>
#include <list>
#include <sstream>
#include <filesystem>

#include "layout/docraft_layout_engine.h"
#include "layout/handler/docraft_layout_handler.h"
#include "renderer/docraft_pdf_renderer.h"
#include "utils/docraft_font_registry.h"
#include "utils/docraft_logger.h"

namespace docraft {
    DocraftDocument::DocraftDocument(std::string document_title) : document_title_(std::move(document_title)) {
    }

    void DocraftDocument::add_node(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node) {
            throw std::runtime_error("Null node cannot be added to the document");
        }
        dom_.emplace_back(node);
    }

    void DocraftDocument::configure_document_settings() const {
        if (settings_) {
            // Apply settings font
            if (!settings_->fonts().empty()) {
                for (const auto &font: settings_->fonts()) {
                    for (const auto &external_font: font.external_fonts) {
                        //read file content
                        auto font_path = external_font.path;
                        // Try to resolve relative paths: prefer absolute as-is, otherwise attempt cwd and executable directory
                        std::filesystem::path resolved_path(font_path);
                        if (!resolved_path.is_absolute()) {
                            std::filesystem::path tried1 = std::filesystem::current_path() / resolved_path;
                            if (std::filesystem::exists(tried1)) {
                                resolved_path = tried1;
                            } else {
                                // try executable directory (approximation: parent of current path)
                                std::filesystem::path tried2 = std::filesystem::current_path().parent_path() / resolved_path;
                                if (std::filesystem::exists(tried2)) {
                                    resolved_path = tried2;
                                }
                            }
                        }

                        bool ok = utils::DocraftFontRegistry::instance().register_font(external_font.name, resolved_path.string());
                        if (!ok) {
                            LOG_ERROR("Failed to register font '" + external_font.name + "' from path '" + font_path + "' (resolved: '" + resolved_path.string() + "')");
                        } else {
                            LOG_DEBUG("Registered font '" + external_font.name + "' from path '" + resolved_path.string() + "'");
                        }
                    }
                }
            }
        }
    }

    void DocraftDocument::render() {
        pdf_context_ = std::make_shared<DocraftDocumentContext>();
        pdf_context_->set_renderer(std::make_shared<renderer::DocraftPDFRenderer>(pdf_context_));
        pdf_context_->set_font_applier(std::make_shared<generic::DocraftFontApplier>(pdf_context_));
        LOG_DEBUG("Rendering document: " + document_title_);

        //Load settings
        configure_document_settings();

        // Layout phase
        layout::DocraftLayoutEngine layout_engine(pdf_context_);
        layout_engine.compute_document_layout(dom_);

        // Rendering phase
        for (auto &node : dom_) {
            if (node) {
                node->draw(pdf_context_);
            }
        }

        pdf_context_->rendering_backend()->save_to_file(document_title_ + ".pdf");
    }

    void DocraftDocument::set_document_title(const std::string &document_title) {
        document_title_ = document_title;
    }

    std::string DocraftDocument::document_title() {
        return document_title_;
    }

    void DocraftDocument::set_settings(const std::shared_ptr<model::DocraftSettings> &settings) {
        settings_ = settings;
    }

    std::shared_ptr<model::DocraftSettings> DocraftDocument::settings() const {
        return settings_;
    }
} // docraft

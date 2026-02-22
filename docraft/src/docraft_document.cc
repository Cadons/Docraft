//
// Created by Matteo on 28/12/25.
//

#include "docraft_document.h"

#include <algorithm>
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
    namespace {
        void apply_page_format_settings(const std::shared_ptr<model::DocraftSettings> &settings,
                                        const std::shared_ptr<DocraftDocumentContext> &context) {
            if (!settings || !context) {
                return;
            }
            if (settings->has_page_format()) {
                context->set_page_format(settings->page_size(), settings->page_orientation());
            }
        }

        void apply_section_ratio_settings(const std::shared_ptr<model::DocraftSettings> &settings,
                                          const std::shared_ptr<DocraftDocumentContext> &context) {
            if (!settings || !context) {
                return;
            }
            if (settings->has_section_ratios()) {
                context->set_section_ratios(settings->header_ratio(),
                                            settings->body_ratio(),
                                            settings->footer_ratio());
            }
        }

        void apply_font_settings(const std::shared_ptr<model::DocraftSettings> &settings) {
            if (!settings) {
                return;
            }
            if (settings->fonts().empty()) {
                return;
            }
            for (const auto &font: settings->fonts()) {
                for (const auto &external_font: font.external_fonts) {
                    auto font_path = external_font.path;
                    std::filesystem::path resolved_path(font_path);
                    if (!resolved_path.is_absolute()) {
                        std::filesystem::path tried1 = std::filesystem::current_path() / resolved_path;
                        if (std::filesystem::exists(tried1)) {
                            resolved_path = tried1;
                        } else {
                            std::filesystem::path tried2 =
                                    std::filesystem::current_path().parent_path() / resolved_path;
                            if (std::filesystem::exists(tried2)) {
                                resolved_path = tried2;
                            }
                        }
                    }

                    bool ok = utils::DocraftFontRegistry::instance().register_font(
                        external_font.name, resolved_path.string());
                    if (!ok) {
                        LOG_ERROR("Failed to register font '" + external_font.name + "' from path '" + font_path +
                            "' (resolved: '" + resolved_path.string() + "')");
                    } else {
                        LOG_DEBUG(
                            "Registered font '" + external_font.name + "' from path '" + resolved_path.string() + "'");
                    }
                }
            }
        }
    } // namespace

    DocraftDocument::DocraftDocument(std::string document_title) : document_title_(std::move(document_title)) {
    }

    void DocraftDocument::add_node(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node) {
            throw std::runtime_error("Null node cannot be added to the document");
        }
        dom_.emplace_back(node);
    }

    void DocraftDocument::configure_document_settings() {
        apply_page_format_settings(settings_, pdf_context_);
        apply_section_ratio_settings(settings_, pdf_context_);
        apply_font_settings(settings_);
    }

    void DocraftDocument::template_document() {
        if (template_engine_) {
            LOG_DEBUG("Template document: '" + document_title_ + "'");
            template_engine_->template_nodes(dom_);
        }
    }

    void DocraftDocument::render() {
        pdf_context_ = std::make_shared<DocraftDocumentContext>();
        pdf_context_->set_renderer(std::make_shared<renderer::DocraftPDFRenderer>(pdf_context_));
        pdf_context_->set_font_applier(std::make_shared<generic::DocraftFontApplier>(pdf_context_));
        LOG_DEBUG("Rendering document: " + document_title_);

        //Load settings
        configure_document_settings();

        //replace template variables in the DOM
        //TODO: here if there is a foreach template engine must add to the defined structure the same node with different data
        template_document();

        // Layout phase
        layout::DocraftLayoutEngine layout_engine(pdf_context_);
        layout_engine.compute_document_layout(dom_);

        // Rendering phase
        const auto &page_backend = pdf_context_->page_backend();
        if (page_backend) {
            page_backend->go_to_first_page();
        }
        const std::size_t page_count = page_backend ? page_backend->total_page_count() : 1;

        for (auto &node: dom_) {
            if (!node) {
                continue;
            }
            if (node->visible()) {
                if (node->page_owner() == -1 && page_backend) {
                    for (std::size_t i = 0; i < page_count; ++i) {
                        page_backend->go_to_page(i);
                        node->draw(pdf_context_);
                    }
                } else {
                    if (page_backend && node->page_owner() > 0) {
                        page_backend->go_to_page(static_cast<std::size_t>(node->page_owner() - 1));
                    }
                    if (node->should_render(pdf_context_)) {
                        node->draw(pdf_context_);
                    }
                }
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

    void DocraftDocument::set_document_template_engine(
        const std::shared_ptr<templating::DocraftTemplateEngine> &template_engine) {
        template_engine_ = template_engine;
    }

    std::shared_ptr<templating::DocraftTemplateEngine> DocraftDocument::document_template_engine() const {
        return template_engine_;
    }

    const std::vector<std::shared_ptr<model::DocraftNode>> &DocraftDocument::nodes() const {
        return dom_;
    }
    std::vector<std::shared_ptr<model::DocraftNode>> DocraftDocument::get_by_name(const std::string &name) const {
        static std::vector<std::shared_ptr<model::DocraftNode>> empty_result;
        std::vector<std::shared_ptr<model::DocraftNode>> result;
        traverse_dom([&](const std::shared_ptr<model::DocraftNode> &node, DocraftDomTraverseOp op) {
            if (op != DocraftDomTraverseOp::kEnter) {
                return;
            }
            if (node && node->node_name() == name) {
                result.push_back(node);
            }
        });
        return result.empty() ? empty_result : result;
    }

    std::shared_ptr<model::DocraftNode> DocraftDocument::get_first_by_name(const std::string &name) const {
        return get_by_name(name).empty() ? nullptr : get_by_name(name).front();
    }

    std::shared_ptr<model::DocraftNode> DocraftDocument::get_last_by_name(const std::string &name) const {
        return get_by_name(name).empty() ? nullptr : get_by_name(name).back();
    }

    void DocraftDocument::traverse_dom(
        const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback) const {
        for (const auto &node: dom_) {
            traverse_node(node, callback);
        }
    }

    void DocraftDocument::traverse_node(
        const std::shared_ptr<model::DocraftNode> &node,
        const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback) const {
        if (!node) {
            return;
        }
        callback(node, DocraftDomTraverseOp::kEnter);
        if (auto parent_node = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            for (const auto &child_node: parent_node->children()) {
                traverse_node(child_node, callback);
            }
        }
        callback(node, DocraftDomTraverseOp::kExit);
    }
} // docraft

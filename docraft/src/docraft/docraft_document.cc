/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "docraft/docraft_document.h"

#include <optional>
#include <iostream>
#include <filesystem>
#include <string_view>
#include <unordered_set>

#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/layout/handler/docraft_layout_handler.h"
#include "docraft/backend/pdf/docraft_haru_backend_providers_factory.h"
#include "docraft/renderer/docraft_pdf_renderer.h"
#include "docraft/utils/docraft_font_registry.h"
#include "docraft/utils/docraft_logger.h"
#include "docraft/model/docraft_settings.h"
#include "docraft/templating/docraft_template_engine.h"

namespace docraft {
    namespace {
        void apply_backend_providers_from_factory(const std::shared_ptr<DocraftDocumentContext> &context,
                                                  const std::shared_ptr<backend::IDocraftBackendProvidersFactory>
                                                  &factory) {
            if (!context || !factory) {
                return;
            }
            auto &rendering_service = context->edit_rendering();
            auto &layout_service = context->edit_layout();
            rendering_service.set_backend_providers_factory(factory);
            if (const auto page_backend = rendering_service.page_rendering()) {
                layout_service.set_page_dimensions(page_backend->page_width(), page_backend->page_height());
            }
        }

        std::string trim_copy(std::string_view source) {
            std::size_t start = 0;
            std::size_t end = source.size();
            while (start < end && std::isspace(static_cast<unsigned char>(source[start]))) {
                ++start;
            }
            while (end > start && std::isspace(static_cast<unsigned char>(source[end - 1]))) {
                --end;
            }
            return std::string(source.substr(start, end - start));
        }

        std::vector<std::string> split_keywords(const std::string &value) {
            std::vector<std::string> keywords;
            std::string current;
            for (const char ch: value) {
                if (ch == ',' || ch == ';') {
                    const std::string candidate = trim_copy(current);
                    if (!candidate.empty()) {
                        keywords.push_back(candidate);
                    }
                    current.clear();
                } else {
                    current.push_back(ch);
                }
            }
            const std::string candidate = trim_copy(current);
            if (!candidate.empty()) {
                keywords.push_back(candidate);
            }
            return keywords;
        }

        std::string normalize_token(const std::string &token) {
            std::string normalized = trim_copy(token);
            for (char &ch: normalized) {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
            return normalized;
        }

        std::string merge_keywords(const std::optional<std::string> &existing_keywords,
                                   const std::vector<std::string> &generated_keywords) {
            std::vector<std::string> merged;
            std::unordered_set<std::string> seen;

            if (existing_keywords && !existing_keywords->empty()) {
                for (const std::string &candidate: split_keywords(*existing_keywords)) {
                    const std::string normalized = normalize_token(candidate);
                    if (normalized.empty()) {
                        continue;
                    }
                    if (seen.insert(normalized).second) {
                        merged.push_back(candidate);
                    }
                }
            }

            for (const std::string &generated: generated_keywords) {
                const std::string normalized = normalize_token(generated);
                if (normalized.empty()) {
                    continue;
                }
                if (seen.insert(normalized).second) {
                    merged.push_back(generated);
                }
            }

            std::string result;
            for (std::size_t i = 0; i < merged.size(); ++i) {
                if (i > 0) {
                    result += ", ";
                }
                result += merged[i];
            }
            return result;
        }

        std::string with_extension(const std::string &base_name, const std::string &extension) {
            if (extension.empty()) {
                return base_name;
            }
            if (extension.front() == '.') {
                return base_name + extension;
            }
            return base_name + "." + extension;
        }

        std::string with_directory(const std::string &directory, const std::string &file_name) {
            if (directory.empty()) {
                return file_name;
            }
            return (std::filesystem::path(directory) / file_name).string();
        }

        void apply_page_format_settings(const std::shared_ptr<model::DocraftSettings> &settings,
                                        const std::shared_ptr<DocraftDocumentContext> &context) {
            if (!settings || !context) {
                return;
            }
            if (settings->has_page_format()) {
                auto &rendering_service = context->edit_rendering();
                auto &layout_service = context->edit_layout();
                if (const auto page_backend = rendering_service.edit_page_rendering()) {
                    page_backend->set_page_format(settings->page_size(), settings->page_orientation());
                    layout_service.set_page_dimensions(page_backend->page_width(), page_backend->page_height());
                } else {
                    layout_service.set_page_format(settings->page_size(), settings->page_orientation());
                }
            }
        }

        void apply_section_ratio_settings(const std::shared_ptr<model::DocraftSettings> &settings,
                                          const std::shared_ptr<DocraftDocumentContext> &context) {
            if (!settings || !context) {
                return;
            }
            if (settings->has_section_ratios()) {
                context->edit_navigation().set_section_ratios(settings->header_ratio(),
                                                              settings->body_ratio(),
                                                              settings->footer_ratio());
            }
        }

        std::filesystem::path resolve_font_path(const std::string &font_path) {
            std::filesystem::path resolved_path(font_path);
            if (resolved_path.is_absolute()) {
                return resolved_path;
            }

            const auto current_directory = std::filesystem::current_path();
            auto current_directory_candidate = current_directory / resolved_path;
            if (std::filesystem::exists(current_directory_candidate)) {
                return current_directory_candidate;
            }

            auto parent_directory_candidate = current_directory.parent_path() / resolved_path;
            if (std::filesystem::exists(parent_directory_candidate)) {
                return parent_directory_candidate;
            }

            return resolved_path;
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
                    const std::string font_path = external_font.path;
                    const std::filesystem::path resolved_path = resolve_font_path(font_path);

                    const bool ok = utils::DocraftFontRegistry::instance().register_font(
                        external_font.name, resolved_path.string());

                    if (!ok) {
                        LOG_ERROR("Failed to register font '" + external_font.name + "' from path '" + font_path +
                            "' (resolved: '" + resolved_path.string() + "')");
                        continue;
                    }

                    LOG_DEBUG(
                        "Registered font '" + external_font.name + "' from path '" + resolved_path.string() + "'");
                }
            }
        }
    } // namespace

    DocraftDocument::DocraftDocument(const std::string &document_title) {
        config_.set_document_title(document_title);
        context_ = std::make_shared<DocraftDocumentContext>();
        backend_providers_factory_ = std::make_shared<backend::pdf::DocraftHaruBackendProvidersFactory>();
        apply_backend_providers_from_factory(context_, backend_providers_factory_);
    }

    void DocraftDocument::add_node(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node) {
            throw std::runtime_error("Null node cannot be added to the document");
        }
        dom_.emplace_back(node);
    }

    void DocraftDocument::configure_document_settings() {
        apply_page_format_settings(config_.edit_settings(), context_);
        apply_section_ratio_settings(config_.edit_settings(), context_);
        apply_font_settings(config_.edit_settings());
    }

    void DocraftDocument::template_document() {
        if (config_.edit_document_template_engine()) {
            LOG_DEBUG("Template document: '" + config_.document_title() + "'");
            config_.edit_document_template_engine()->template_nodes(dom_);
        }
    }

    void DocraftDocument::handle_node_rendering(const std::shared_ptr<model::DocraftNode> &node) {
        auto &rendering_service = context_->edit_rendering();
        auto page_backend = rendering_service.edit_page_rendering();
        const std::size_t page_count = page_backend ? page_backend->total_page_count() : 1;
        if (node->page_owner() == -1 && page_backend) {
            for (std::size_t i = 0; i < page_count; ++i) {
                page_backend->go_to_page(i);
                node->draw(context_);
            }
        } else {
            if (page_backend && node->page_owner() > 0) {
                page_backend->go_to_page(static_cast<std::size_t>(node->page_owner() - 1));
            }
            if (node->should_render(context_)) {
                node->draw(context_);
            }
        }
    }

    void DocraftDocument::render() {
        apply_backend_providers_from_factory(context_, backend_providers_factory_);
        context_->set_renderer(std::make_shared<renderer::DocraftPDFRenderer>(context_));
        context_->edit_typography().set_font_applier(std::make_shared<generic::DocraftFontApplier>(context_));
        LOG_DEBUG("Rendering document: " + config_.document_title());

        configure_document_settings();
        template_document();

        if (config_.auto_keywords_enabled()) {
            utils::DocraftKeywordExtractor extractor(config_.auto_keywords_config());
            const std::vector<std::string> extracted_keywords = extractor.extract(*this);
            if (!extracted_keywords.empty()) {
                auto metadata = config_.document_metadata();
                metadata.set_keywords(merge_keywords(metadata.keywords(), extracted_keywords));
                config_.set_document_metadata(metadata);
            }
        }

        layout::DocraftLayoutEngine layout_engine(context_);
        layout_engine.compute_document_layout(dom_);

        const auto page_backend = context_->edit_rendering().edit_page_rendering();
        if (page_backend) {
            page_backend->go_to_first_page();
        }

        for (auto &node: dom_) {
            if (!node) {
                continue;
            }
            if (node->visible()) {
                handle_node_rendering(node);
            }
        }

        auto &rendering_service = context_->edit_rendering();

        const auto metadata_backend = rendering_service.edit_metadata_backend();
        if (!metadata_backend) {
            throw std::runtime_error("Metadata backend capability is not available");
        }
        metadata_backend->set_document_metadata(config_.document_metadata());

        const auto output_backend = rendering_service.output_backend();
        if (!output_backend) {
            throw std::runtime_error("Output backend capability is not available");
        }
        const std::string extension = output_backend->file_extension();
        const std::string output_file_name = with_extension(config_.document_title(), extension);
        const std::string output_path = with_directory(config_.document_path(), output_file_name);
        output_backend->save_to_file(output_path);
    }

    void DocraftDocument::set_backend_providers_factory(
        const std::shared_ptr<backend::IDocraftBackendProvidersFactory> &backend_providers_factory) {
        backend_providers_factory_ = backend_providers_factory
                                         ? backend_providers_factory
                                         : std::make_shared<backend::pdf::DocraftHaruBackendProvidersFactory>();
        apply_backend_providers_from_factory(context_, backend_providers_factory_);
    }

    std::vector<std::shared_ptr<const model::DocraftNode> > DocraftDocument::nodes() const {
        return docraft::to_const_shared_vector(dom_);
    }

    std::vector<std::shared_ptr<model::DocraftNode> > &DocraftDocument::edit_nodes() {
        return dom_;
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

    management::DocraftDocumentConfig &DocraftDocument::edit_config() {
        return config_;
    }

    const management::DocraftDocumentConfig &DocraftDocument::config() const {
        return config_;
    }

    std::shared_ptr<DocraftDocumentContext> DocraftDocument::edit_context() {
        return context_;
    }

    std::shared_ptr<const DocraftDocumentContext> DocraftDocument::context() const {
        return context_;
    }

    // Backward compatibility delegates to config_
    void DocraftDocument::set_document_title(const std::string &document_title) {
        config_.set_document_title(document_title);
    }

    const std::string &DocraftDocument::document_title() const {
        return config_.document_title();
    }

    void DocraftDocument::set_document_path(const std::string &document_path) {
        config_.set_document_path(document_path);
    }

    const std::string &DocraftDocument::document_path() const {
        return config_.document_path();
    }

    void DocraftDocument::set_settings(const std::shared_ptr<model::DocraftSettings> &settings) {
        config_.set_settings(settings);
    }

    std::shared_ptr<const model::DocraftSettings> DocraftDocument::settings() const {
        return config_.settings();
    }

    void DocraftDocument::set_document_metadata(const DocraftDocumentMetadata &metadata) {
        config_.set_document_metadata(metadata);
    }

    const DocraftDocumentMetadata &DocraftDocument::document_metadata() const {
        return config_.document_metadata();
    }

    // Backward compatibility: DOM query methods
    std::vector<std::shared_ptr<const model::DocraftNode> >
    DocraftDocument::find_by_name(const std::string &name) const {
        return management::DocraftDocumentQuery::find_by_name(
            dom_, name);
    }

    std::vector<std::shared_ptr<model::DocraftNode> > DocraftDocument::take_by_name(const std::string &name) {
        return management::DocraftDocumentQuery::take_by_name(dom_, name);
    }

    std::shared_ptr<const model::DocraftNode> DocraftDocument::find_first_by_name(const std::string &name) const {
        return management::DocraftDocumentQuery::find_first_by_name(
            dom_, name);
    }

    std::shared_ptr<model::DocraftNode> DocraftDocument::take_first_by_name(const std::string &name) {
        return management::DocraftDocumentQuery::take_first_by_name(dom_, name);
    }

    std::shared_ptr<const model::DocraftNode> DocraftDocument::find_last_by_name(const std::string &name) const {
        return management::DocraftDocumentQuery::find_last_by_name(
            dom_, name);
    }

    std::shared_ptr<model::DocraftNode> DocraftDocument::take_last_by_name(const std::string &name) {
        return management::DocraftDocumentQuery::take_last_by_name(dom_, name);
    }

    // Backward compatibility: config shortcuts
    void DocraftDocument::enable_auto_keywords(bool enabled) {
        config_.enable_auto_keywords(enabled);
    }

    bool DocraftDocument::auto_keywords_enabled() const {
        return config_.auto_keywords_enabled();
    }

    void DocraftDocument::set_auto_keywords_config(const utils::DocraftKeywordExtractor::Config &config) {
        config_.set_auto_keywords_config(config);
    }

    const utils::DocraftKeywordExtractor::Config &DocraftDocument::auto_keywords_config() const {
        return config_.auto_keywords_config();
    }

    void DocraftDocument::set_document_template_engine(
        const std::shared_ptr<templating::DocraftTemplateEngine> &template_engine) {
        config_.set_document_template_engine(template_engine);
    }

    std::shared_ptr<const templating::DocraftTemplateEngine> DocraftDocument::document_template_engine() const {
        return config_.document_template_engine();
    }

    std::shared_ptr<templating::DocraftTemplateEngine> DocraftDocument::edit_document_template_engine() {
        return config_.edit_document_template_engine();
    }

    void DocraftDocument::refresh_auto_keywords() {
        if (!config_.auto_keywords_enabled()) {
            return;
        }
        utils::DocraftKeywordExtractor extractor(config_.auto_keywords_config());
        const std::vector<std::string> extracted_keywords = extractor.extract(*this);
        if (extracted_keywords.empty()) {
            return;
        }
        auto metadata = config_.document_metadata();
        metadata.set_keywords(merge_keywords(metadata.keywords(), extracted_keywords));
        config_.set_document_metadata(metadata);
    }
} // docraft

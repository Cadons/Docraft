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

#include "docraft/docraft_document_context.h"
#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/management/docraft_backend_cache.h"
#include "docraft/management/docraft_document_section_manager.h"

namespace docraft {
    DocraftDocumentContext::DocraftDocumentContext() {
        backend_ = std::make_shared<backend::pdf::DocraftHaruBackend>();
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();
        current_rect_width_ = page_width_;
        refresh_backend_caches();
    }

    DocraftDocumentContext::DocraftDocumentContext(
        const std::shared_ptr<backend::IDocraftRenderingBackend> &backend) : backend_(
        backend) {
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();
        current_rect_width_ = page_width_;
        refresh_backend_caches();
    }

    DocraftDocumentContext::~DocraftDocumentContext() = default;
#pragma region setter
    void DocraftDocumentContext::set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer) {
        renderer_ = renderer;
    }

    void DocraftDocumentContext::set_current_rect_width(float current_rect_width) {
        current_rect_width_ = current_rect_width;
    }

    std::shared_ptr<const generic::DocraftFontApplier> DocraftDocumentContext::font_applier() const {
        return font_applier_;
    }

    std::shared_ptr<generic::DocraftFontApplier> DocraftDocumentContext::edit_font_applier() {
        return font_applier_;
    }

    void DocraftDocumentContext::refresh_backend_caches() {
        backend_cache_.initialize_from_backend(backend_);
    }

    management::DocraftDocumentSectionManager &DocraftDocumentContext::section_manager() {
        return section_manager_;
    }

    const management::DocraftDocumentSectionManager &DocraftDocumentContext::section_manager() const {
        return section_manager_;
    }

    management::DocraftBackendCache &DocraftDocumentContext::backend_cache() {
        return backend_cache_;
    }

    const management::DocraftBackendCache &DocraftDocumentContext::backend_cache() const {
        return backend_cache_;
    }

    void DocraftDocumentContext::set_backend(const std::shared_ptr<backend::IDocraftRenderingBackend> &backend) {
        backend_ = backend ? backend : std::make_shared<backend::pdf::DocraftHaruBackend>();
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();
        current_rect_width_ = page_width_;
        refresh_backend_caches();
    }

    void DocraftDocumentContext::set_page_format(model::DocraftPageSize size,
                                                 model::DocraftPageOrientation orientation) {
        const auto backend = backend_cache_.edit_page_backend();
        if (backend) {
            backend->set_page_format(size, orientation);
            page_height_ = backend->page_height();
            page_width_ = backend->page_width();
            current_rect_width_ = page_width_;
        }
    }

    void DocraftDocumentContext::set_font_applier(const std::shared_ptr<generic::DocraftFontApplier> &font_applier) {
        font_applier_ = font_applier;
    }
#pragma endregion
#pragma region getter
    std::shared_ptr<const backend::IDocraftRenderingBackend> DocraftDocumentContext::rendering_backend() const {
        return backend_;
    }

    std::shared_ptr<backend::IDocraftRenderingBackend> DocraftDocumentContext::edit_rendering_backend() {
        return backend_;
    }

    DocraftCursor &DocraftDocumentContext::cursor() {
        return cursor_;
    }

    float DocraftDocumentContext::available_space() const {
        return current_rect_width_;
    }

    std::shared_ptr<renderer::DocraftAbstractRenderer> DocraftDocumentContext::renderer() {
        if (!renderer_) {
            throw std::runtime_error("docraft/renderer not set in DocraftPDFContext");
        }
        return renderer_;
    }

    float DocraftDocumentContext::page_height() const {
        return page_height_;
    }

    float DocraftDocumentContext::page_width() const {
        return page_width_;
    }

    void DocraftDocumentContext::go_to_first_page() {
        const auto backend = backend_cache_.edit_page_backend();
        if (backend) {
            backend->go_to_first_page();
        }
    }

    void DocraftDocumentContext::go_to_previous_page() {
        const auto backend = backend_cache_.edit_page_backend();
        if (backend) {
            backend->go_to_previous_page();
        }
    }

    void DocraftDocumentContext::go_to_last_page() {
        const auto backend = backend_cache_.edit_page_backend();
        if (backend) {
            backend->go_to_last_page();
        }
    }

    // Backward compatibility delegates to backend_cache()
    std::shared_ptr<const backend::IDocraftLineRenderingBackend> DocraftDocumentContext::line_backend() const {
        return backend_cache_.line_backend();
    }

    std::shared_ptr<backend::IDocraftLineRenderingBackend> DocraftDocumentContext::edit_line_backend() {
        return backend_cache_.edit_line_backend();
    }

    std::shared_ptr<const backend::IDocraftShapeRenderingBackend> DocraftDocumentContext::shape_backend() const {
        return backend_cache_.shape_backend();
    }

    std::shared_ptr<backend::IDocraftShapeRenderingBackend> DocraftDocumentContext::edit_shape_backend() {
        return backend_cache_.edit_shape_backend();
    }

    std::shared_ptr<const backend::IDocraftTextRenderingBackend> DocraftDocumentContext::text_backend() const {
        return backend_cache_.text_backend();
    }

    std::shared_ptr<backend::IDocraftTextRenderingBackend> DocraftDocumentContext::edit_text_backend() {
        return backend_cache_.edit_text_backend();
    }

    std::shared_ptr<const backend::IDocraftImageRenderingBackend> DocraftDocumentContext::image_backend() const {
        return backend_cache_.image_backend();
    }

    std::shared_ptr<backend::IDocraftImageRenderingBackend> DocraftDocumentContext::edit_image_backend() {
        return backend_cache_.edit_image_backend();
    }

    std::shared_ptr<const backend::IDocraftPageRenderingBackend> DocraftDocumentContext::page_backend() const {
        return backend_cache_.page_backend();
    }

    std::shared_ptr<backend::IDocraftPageRenderingBackend> DocraftDocumentContext::edit_page_backend() {
        return backend_cache_.edit_page_backend();
    }

    // Backward compatibility delegates to section_manager()
    void DocraftDocumentContext::set_header(const std::shared_ptr<model::DocraftHeader> &header) {
        section_manager_.set_header(header);
    }

    std::shared_ptr<const model::DocraftHeader> DocraftDocumentContext::header() const {
        return section_manager_.header();
    }

    std::shared_ptr<model::DocraftHeader> DocraftDocumentContext::edit_header() {
        return section_manager_.edit_header();
    }

    void DocraftDocumentContext::set_body(const std::shared_ptr<model::DocraftBody> &body) {
        section_manager_.set_body(body);
    }

    std::shared_ptr<const model::DocraftBody> DocraftDocumentContext::body() const {
        return section_manager_.body();
    }

    std::shared_ptr<model::DocraftBody> DocraftDocumentContext::edit_body() {
        return section_manager_.edit_body();
    }

    void DocraftDocumentContext::set_footer(const std::shared_ptr<model::DocraftFooter> &footer) {
        section_manager_.set_footer(footer);
    }

    std::shared_ptr<const model::DocraftFooter> DocraftDocumentContext::footer() const {
        return section_manager_.footer();
    }

    std::shared_ptr<model::DocraftFooter> DocraftDocumentContext::edit_footer() {
        return section_manager_.edit_footer();
    }

    void DocraftDocumentContext::set_section_ratios(float header_ratio, float body_ratio, float footer_ratio) {
        section_manager_.set_section_ratios(header_ratio, body_ratio, footer_ratio);
    }

    float DocraftDocumentContext::header_ratio() const {
        return section_manager_.header_ratio();
    }

    float DocraftDocumentContext::body_ratio() const {
        return section_manager_.body_ratio();
    }

    float DocraftDocumentContext::footer_ratio() const {
        return section_manager_.footer_ratio();
    }
#pragma endregion
} // docraft

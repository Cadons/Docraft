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

namespace docraft {
    DocraftDocumentContext::DocraftDocumentContext() {
        backend_ = std::make_shared<backend::pdf::DocraftHaruBackend>();
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();
        current_rect_width_ = page_width_;
        refresh_backend_caches_();
    }

    DocraftDocumentContext::DocraftDocumentContext(
        const std::shared_ptr<backend::IDocraftRenderingBackend> &backend) : backend_(
        backend) {
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();
        current_rect_width_ = page_width_;
        refresh_backend_caches_();
    }

    DocraftDocumentContext::~DocraftDocumentContext() = default;
#pragma region setter
    void DocraftDocumentContext::set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer) {
        renderer_ = renderer;
    }

    void DocraftDocumentContext::set_current_rect_width(float current_rect_width) {
        current_rect_width_ = current_rect_width;
    }

    void DocraftDocumentContext::set_header(const std::shared_ptr<model::DocraftHeader> &header) {
        header_ = header;
    }

    void DocraftDocumentContext::set_body(const std::shared_ptr<model::DocraftBody> &body) {
        body_ = body;
    }

    void DocraftDocumentContext::set_footer(const std::shared_ptr<model::DocraftFooter> &footer) {
        footer_ = footer;
    }

    void DocraftDocumentContext::set_font_applier(const std::shared_ptr<generic::DocraftFontApplier> &font_applier) {
        font_applier_ = font_applier;
    }

    void DocraftDocumentContext::refresh_backend_caches_() {
        docraft::ensure_lazy_backend<backend::IDocraftLineRenderingBackend>(line_backend_, backend_);
        docraft::ensure_lazy_backend<backend::IDocraftShapeRenderingBackend>(shape_backend_, backend_);
        docraft::ensure_lazy_backend<backend::IDocraftTextRenderingBackend>(text_backend_, backend_);
        docraft::ensure_lazy_backend<backend::IDocraftImageRenderingBackend>(image_backend_, backend_);
        docraft::ensure_lazy_backend<backend::IDocraftPageRenderingBackend>(page_backend_, backend_);
    }

    void DocraftDocumentContext::set_backend(const std::shared_ptr<backend::IDocraftRenderingBackend> &backend) {
        backend_ = backend ? backend : std::make_shared<backend::pdf::DocraftHaruBackend>();
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();
        current_rect_width_ = page_width_;
        refresh_backend_caches_();
    }

    void DocraftDocumentContext::set_page_format(model::DocraftPageSize size,
                                                 model::DocraftPageOrientation orientation) {
        const auto backend = edit_page_backend();
        if (backend) {
            backend->set_page_format(size, orientation);
            page_height_ = backend->page_height();
            page_width_ = backend->page_width();
            current_rect_width_ = page_width_;
        }
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

    std::shared_ptr<const model::DocraftHeader> DocraftDocumentContext::header() const {
        return header_;
    }

    std::shared_ptr<model::DocraftHeader> DocraftDocumentContext::edit_header() {
        return header_;
    }

    std::shared_ptr<const model::DocraftBody> DocraftDocumentContext::body() const {
        return body_;
    }

    std::shared_ptr<model::DocraftBody> DocraftDocumentContext::edit_body() {
        return body_;
    }

    std::shared_ptr<const model::DocraftFooter> DocraftDocumentContext::footer() const {
        return footer_;
    }

    std::shared_ptr<model::DocraftFooter> DocraftDocumentContext::edit_footer() {
        return footer_;
    }

    std::shared_ptr<const generic::DocraftFontApplier> DocraftDocumentContext::font_applier() const {
        return font_applier_;
    }

    std::shared_ptr<generic::DocraftFontApplier> DocraftDocumentContext::edit_font_applier() {
        return font_applier_;
    }

    std::shared_ptr<const backend::IDocraftLineRenderingBackend> DocraftDocumentContext::line_backend() const {
        return line_backend_;
    }

    std::shared_ptr<backend::IDocraftLineRenderingBackend> DocraftDocumentContext::edit_line_backend() {
        return line_backend_;
    }

    std::shared_ptr<const backend::IDocraftShapeRenderingBackend> DocraftDocumentContext::shape_backend() const {
        return shape_backend_;
    }

    std::shared_ptr<backend::IDocraftShapeRenderingBackend> DocraftDocumentContext::edit_shape_backend() {
        return shape_backend_;
    }

    std::shared_ptr<const backend::IDocraftTextRenderingBackend> DocraftDocumentContext::text_backend() const {
        return text_backend_;
    }

    std::shared_ptr<backend::IDocraftTextRenderingBackend> DocraftDocumentContext::edit_text_backend() {
        return text_backend_;
    }

    std::shared_ptr<const backend::IDocraftImageRenderingBackend> DocraftDocumentContext::image_backend() const {
        return image_backend_;
    }

    std::shared_ptr<backend::IDocraftImageRenderingBackend> DocraftDocumentContext::edit_image_backend() {
        return image_backend_;
    }

    std::shared_ptr<const backend::IDocraftPageRenderingBackend> DocraftDocumentContext::page_backend() const {
        return page_backend_;
    }

    std::shared_ptr<backend::IDocraftPageRenderingBackend> DocraftDocumentContext::edit_page_backend() {
        return page_backend_;
    }

    void DocraftDocumentContext::go_to_first_page() {
        const auto backend = edit_page_backend();
        if (backend) {
            backend->go_to_first_page();
        }
    }

    void DocraftDocumentContext::go_to_previous_page() {
        const auto backend = edit_page_backend();
        if (backend) {
            backend->go_to_previous_page();
        }
    }

    void DocraftDocumentContext::go_to_last_page() {
        const auto backend = edit_page_backend();
        if (backend) {
            backend->go_to_last_page();
        }
    }

    void DocraftDocumentContext::set_section_ratios(float header_ratio, float body_ratio, float footer_ratio) {
        header_ratio_ = header_ratio;
        body_ratio_ = body_ratio;
        footer_ratio_ = footer_ratio;
    }

    float DocraftDocumentContext::header_ratio() const {
        return header_ratio_;
    }

    float DocraftDocumentContext::body_ratio() const {
        return body_ratio_;
    }

    float DocraftDocumentContext::footer_ratio() const {
        return footer_ratio_;
    }
#pragma endregion
} // docraft

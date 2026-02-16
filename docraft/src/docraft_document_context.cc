#include "docraft_document_context.h"
#include "backend/pdf/docraft_haru_backend.h"

namespace docraft {
    DocraftDocumentContext::DocraftDocumentContext() {
        backend_ = std::make_shared<backend::pdf::DocraftHaruBackend>();
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();

        current_rect_width_ = page_width_;
    }

    DocraftDocumentContext::DocraftDocumentContext(
        const std::shared_ptr<backend::IDocraftRenderingBackend> &backend) : backend_(
        backend) {
        page_height_ = backend_->page_height();
        page_width_ = backend_->page_width();
        current_rect_width_ = page_width_;
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

    void DocraftDocumentContext::set_backend(const std::shared_ptr<backend::IDocraftRenderingBackend> &backend) {
        backend_ = backend;
        line_backend_.reset();
        shape_backend_.reset();
        text_backend_.reset();
        image_backend_.reset();
        page_backend_.reset();
    }

    void DocraftDocumentContext::set_page_format(model::DocraftPageSize size,
                                                 model::DocraftPageOrientation orientation) {
        const auto &backend = page_backend();
        if (backend) {
            backend->set_page_format(size, orientation);
            page_height_ = backend->page_height();
            page_width_ = backend->page_width();
            current_rect_width_ = page_width_;
        }
    }
#pragma endregion
#pragma region getter
    const std::shared_ptr<backend::IDocraftRenderingBackend> &DocraftDocumentContext::rendering_backend() const {
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
            throw std::runtime_error("Renderer not set in DocraftPDFContext");
        }
        return renderer_;
    }

    float DocraftDocumentContext::page_height() const {
        return page_height_;
    }

    float DocraftDocumentContext::page_width() const {
        return page_width_;
    }


    const std::shared_ptr<model::DocraftHeader> &DocraftDocumentContext::header() const {
        return header_;
    }


    const std::shared_ptr<model::DocraftBody> &DocraftDocumentContext::body() const {
        return body_;
    }

    const std::shared_ptr<model::DocraftFooter> &DocraftDocumentContext::footer() const {
        return footer_;
    }

    const std::shared_ptr<generic::DocraftFontApplier> &DocraftDocumentContext::font_applier() const {
        return font_applier_;
    }

    const std::shared_ptr<backend::IDocraftLineRenderingBackend> &DocraftDocumentContext::line_backend() const {
        if (!line_backend_) {
            line_backend_ = std::dynamic_pointer_cast<backend::IDocraftLineRenderingBackend>(backend_);
        }
        return line_backend_;
    }

    const std::shared_ptr<backend::IDocraftShapeRenderingBackend> &DocraftDocumentContext::shape_backend() const {
        if (!shape_backend_) {
            shape_backend_ = std::dynamic_pointer_cast<backend::IDocraftShapeRenderingBackend>(backend_);
        }
        return shape_backend_;
    }

    const std::shared_ptr<backend::IDocraftTextRenderingBackend> &DocraftDocumentContext::text_backend() const {
        if (!text_backend_) {
            text_backend_ = std::dynamic_pointer_cast<backend::IDocraftTextRenderingBackend>(backend_);
        }
        return text_backend_;
    }

    const std::shared_ptr<backend::IDocraftImageRenderingBackend> &DocraftDocumentContext::image_backend() const {
        if (!image_backend_) {
            image_backend_ = std::dynamic_pointer_cast<backend::IDocraftImageRenderingBackend>(backend_);
        }
        return image_backend_;
    }

    const std::shared_ptr<backend::IDocraftPageRenderingBackend> &DocraftDocumentContext::page_backend() const {
        if (!page_backend_) {
            page_backend_ = std::dynamic_pointer_cast<backend::IDocraftPageRenderingBackend>(backend_);
        }
        return page_backend_;
    }

    void DocraftDocumentContext::go_to_first_page() const {
        const auto &backend = page_backend();
        if (backend) {
            backend->go_to_first_page();
        }
    }

    void DocraftDocumentContext::go_to_previous_page() const {
        const auto &backend = page_backend();
        if (backend) {
            backend->go_to_previous_page();
        }
    }

    void DocraftDocumentContext::go_to_last_page() const {
        const auto &backend = page_backend();
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

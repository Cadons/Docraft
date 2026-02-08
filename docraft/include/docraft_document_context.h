#pragma once
#include <hpdf_doc.h>
#include <memory>

#include "docraft_cursor.h"
#include "generic/docraft_font_applier.h"

namespace docraft {
    namespace renderer {
        class DocraftAbstractRenderer;
    }
    namespace model {
        class DocraftHeader;
        class DocraftBody;
        class DocraftFooter;
    }

    class DocraftDocumentContext {
        public:
        DocraftDocumentContext();
        ~DocraftDocumentContext();

        HPDF_Doc pdf_doc() const;

        HPDF_Page page() const;
        DocraftCursor& cursor();
        float available_space() const;
        void set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer);
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer();

        void set_current_rect_width(float x);
        [[nodiscard]] float page_width() const;
        [[nodiscard]] float page_height() const;
        void set_header(const std::shared_ptr<model::DocraftHeader> &header);
        [[nodiscard]] const std::shared_ptr<model::DocraftHeader>& header() const;
        void set_body(const std::shared_ptr<model::DocraftBody> &body);
        [[nodiscard]] const std::shared_ptr<model::DocraftBody>& body() const;
        void set_footer(const std::shared_ptr<model::DocraftFooter> &footer);
        [[nodiscard]] const std::shared_ptr<model::DocraftFooter>& footer() const;
        void set_font_applier(const std::shared_ptr<docraft::generic::DocraftFontApplier>& font_applier);
        [[nodiscard]] const std::shared_ptr<docraft::generic::DocraftFontApplier>& font_applier()const;

    private:
        HPDF_Doc pdf_doc_;
        HPDF_Page page_;
        DocraftCursor cursor_;
        float current_rect_width_=0;
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer_;
        float page_width_;
        float page_height_;
        std::shared_ptr<model::DocraftHeader> header_;
        std::shared_ptr<model::DocraftBody> body_;
        std::shared_ptr<model::DocraftFooter> footer_;
        std::shared_ptr<docraft::generic::DocraftFontApplier> font_applier_;
    };
} // docraft

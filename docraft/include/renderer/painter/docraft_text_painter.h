#pragma once
#include <list>
#include "backend/docraft_text_rendering_backend.h"

#include "model/docraft_text.h"
#include "renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    class DocraftTextPainter : public IPainter{
    public:
        explicit DocraftTextPainter(const model::DocraftText &text_node);
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    protected:
        void render_justified(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);
        std::pair<std::pair<float,float>,std::pair<float,float>> render_text(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);
        std::pair<std::pair<float,float>,std::pair<float,float>> draw_text(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);
        void draw_underline(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);

    private:
        model::DocraftText text_node_;
        std::shared_ptr<model::DocraftText> current_line_;
        std::shared_ptr<docraft::backend::IDocraftTextRenderingBackend> text_backend_;
    };
} // docraft

#pragma once
#include "model/docraft_image.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_table.h"
#include "renderer/docraft_renderer.h"

namespace docraft::renderer {
    class DocraftPDFRenderer : public DocraftAbstractRenderer {
    public:
        using DocraftAbstractRenderer::DocraftAbstractRenderer;
        void render_text(const model::DocraftText &text_node) override;

        void render_section(const model::DocraftSection &section_node) override;

        void render_image(const model::DocraftImage &image_node) override;
        void render_table(const model::DocraftTable &table_node) override;
        void render_rectangle(const model::DocraftRectangle &rectangle_node) override;

        void render_blank_line(const model::DocraftBlankLine &blank_line_node) override;

    };
} // renderer

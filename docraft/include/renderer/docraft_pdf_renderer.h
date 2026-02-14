#pragma once
#include "model/docraft_image.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_table.h"
#include "renderer/docraft_renderer.h"

namespace docraft::renderer {
    /**
     * @brief Concrete renderer that targets a PDF backend.
     *
     * Dispatches each node type to the appropriate painter implementation.
     */
    class DocraftPDFRenderer : public DocraftAbstractRenderer {
    public:
        using DocraftAbstractRenderer::DocraftAbstractRenderer;
        /**
         * @brief Renders a text node to PDF.
         * @param text_node Text node.
         */
        void render_text(const model::DocraftText &text_node) override;

        /**
         * @brief Renders a section node to PDF.
         * @param section_node Section node.
         */
        void render_section(const model::DocraftSection &section_node) override;

        /**
         * @brief Renders an image node to PDF.
         * @param image_node Image node.
         */
        void render_image(const model::DocraftImage &image_node) override;
        /**
         * @brief Renders a table node to PDF.
         * @param table_node Table node.
         */
        void render_table(const model::DocraftTable &table_node) override;
        /**
         * @brief Renders a rectangle node to PDF.
         * @param rectangle_node Rectangle node.
         */
        void render_rectangle(const model::DocraftRectangle &rectangle_node) override;

        /**
         * @brief Renders a blank line node to PDF.
         * @param blank_line_node Blank line node.
         */
        void render_blank_line(const model::DocraftBlankLine &blank_line_node) override;

    };
} // renderer

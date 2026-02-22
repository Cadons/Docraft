#pragma once

#include "docraft_lib.h"
#include <memory>

#include "model/docraft_blank_line.h"
#include "model/docraft_circle.h"
#include "model/docraft_image.h"
#include "model/docraft_line.h"
#include "model/docraft_polygon.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_section.h"
#include "model/docraft_table.h"
#include "model/docraft_triangle.h"

namespace docraft::renderer {
    /**
     * @brief Abstract renderer that converts model nodes into backend draw calls.
     *
     * Concrete renderers (e.g., PDF) implement each node type using painters
     * and the document context's rendering backend.
     */
    class DOCRAFT_LIB DocraftAbstractRenderer {
    public:
        /**
         * @brief Creates a renderer bound to a document context.
         * @param docraftContext Document context used for backend access.
         */
        explicit DocraftAbstractRenderer(const std::shared_ptr<DocraftDocumentContext> &docraftContext) : docraftContext_(
            docraftContext) {
        }

        virtual ~DocraftAbstractRenderer() = default;

        /**
         * @brief Renders a text node.
         * @param text_node Text node.
         */
        virtual void render_text(const model::DocraftText &text_node) = 0;

        /**
         * @brief Renders a section node.
         * @param section_node Section node.
         */
        virtual void render_section(const model::DocraftSection &section_node) = 0;

        /**
         * @brief Renders an image node.
         * @param image_node Image node.
         */
        virtual void render_image(const model::DocraftImage &image_node) = 0;

        /**
         * @brief Renders a table node.
         * @param table_node Table node.
         */
        virtual void render_table(const model::DocraftTable &table_node) = 0;

        /**
         * @brief Renders a rectangle node.
         * @param rectangle_node Rectangle node.
         */
        virtual void render_rectangle(const model::DocraftRectangle &rectangle_node) = 0;
        /**
         * @brief Renders a circle node.
         * @param circle_node Circle node.
         */
        virtual void render_circle(const model::DocraftCircle &circle_node) = 0;
        /**
         * @brief Renders a triangle node.
         * @param triangle_node Triangle node.
         */
        virtual void render_triangle(const model::DocraftTriangle &triangle_node) = 0;
        /**
         * @brief Renders a line node.
         * @param line_node Line node.
         */
        virtual void render_line(const model::DocraftLine &line_node) = 0;
        /**
         * @brief Renders a polygon node.
         * @param polygon_node Polygon node.
         */
        virtual void render_polygon(const model::DocraftPolygon &polygon_node) = 0;

        /**
         * @brief Renders a blank line node.
         * @param blank_line_node Blank line node.
         */
        virtual void render_blank_line(const model::DocraftBlankLine &blank_line_node)=0;

    protected:
        /**
         * @brief Returns the bound document context.
         * @return Document context.
         */
        const std::shared_ptr<DocraftDocumentContext> &context() const {
            return docraftContext_;
        }

    private:
        std::shared_ptr<DocraftDocumentContext> docraftContext_;
    };
} // renderer

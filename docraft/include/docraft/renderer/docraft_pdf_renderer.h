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

#pragma once

#include "docraft/docraft_lib.h"
#include "docraft/model/docraft_circle.h"
#include "docraft/model/docraft_image.h"
#include "docraft/model/docraft_line.h"
#include "docraft/model/docraft_polygon.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_triangle.h"
#include "docraft/renderer/docraft_renderer.h"

namespace docraft::renderer {
    /**
     * @brief Concrete renderer that targets a PDF backend.
     *
     * Dispatches each node type to the appropriate painter implementation.
     */
    class DOCRAFT_LIB DocraftPDFRenderer : public DocraftAbstractRenderer {
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
         * @brief Renders a circle node to PDF.
         * @param circle_node Circle node.
         */
        void render_circle(const model::DocraftCircle &circle_node) override;
        /**
         * @brief Renders a triangle node to PDF.
         * @param triangle_node Triangle node.
         */
        void render_triangle(const model::DocraftTriangle &triangle_node) override;
        /**
         * @brief Renders a line node to PDF.
         * @param line_node Line node.
         */
        void render_line(const model::DocraftLine &line_node) override;
        /**
         * @brief Renders a polygon node to PDF.
         * @param polygon_node Polygon node.
         */
        void render_polygon(const model::DocraftPolygon &polygon_node) override;

        /**
         * @brief Renders a blank line node to PDF.
         * @param blank_line_node Blank line node.
         */
        void render_blank_line(const model::DocraftBlankLine &blank_line_node) override;

    };
} // renderer

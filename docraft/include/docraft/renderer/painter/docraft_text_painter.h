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
#include <list>
#include "docraft/backend/docraft_text_rendering_backend.h"

#include "docraft/model/docraft_text.h"
#include "docraft/renderer/painter/i_painter.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftText nodes.
     *
     * Handles alignment, justification, and optional underline rendering.
     */
    class DOCRAFT_LIB DocraftTextPainter : public IPainter{
    public:
        /**
         * @brief Creates a painter for a given text node.
         * @param text_node Text node to render.
         */
        explicit DocraftTextPainter(const model::DocraftText &text_node);
        /**
         * @brief Draws the text node using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    protected:
        /**
         * @brief Renders justified text within the current layout width.
         * @param context Document context.
         * @param text Text string to render.
         */
        void render_justified(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);
        /**
         * @brief Lays out and renders text, returning start/end cursor positions.
         * @param context Document context.
         * @param text Text string to render.
         * @return Pair of start/end positions (x,y) for the rendered text.
         */
        std::pair<std::pair<float,float>,std::pair<float,float>> render_text(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);
        /**
         * @brief Draws a text run at the current cursor position.
         * @param context Document context.
         * @param text Text string to draw.
         * @return Pair of start/end positions (x,y) for the drawn text.
         */
        std::pair<std::pair<float,float>,std::pair<float,float>> draw_text(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);
        /**
         * @brief Draws an underline under the given text.
         * @param context Document context.
         * @param text Text string to underline.
         */
        void draw_underline(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text);

    private:
        model::DocraftText text_node_;
        std::shared_ptr<model::DocraftText> current_line_;
        std::shared_ptr<docraft::backend::IDocraftTextRenderingBackend> text_backend_;
    };
} // docraft

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
#include <memory>

#include "docraft_cursor.h"
#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/generic/docraft_font_applier.h"
#include "docraft/model/docraft_page_format.h"

namespace docraft {
    namespace renderer {
        class DocraftAbstractRenderer;
    }
    namespace model {
        class DocraftHeader;
        class DocraftBody;
        class DocraftFooter;
    }

    /**
     * @brief Shared rendering and layout state for a document.
     *
     * The context holds the active rendering backend, cached backend interfaces,
     * page metrics, section nodes, and the layout cursor used by the engine.
     */
    class DOCRAFT_LIB DocraftDocumentContext {
        public:
        /**
         * @brief Constructs a context with a default backend.
         */
        DocraftDocumentContext();
        /**
         * @brief Constructs a context with the provided rendering backend.
         * @param backend Rendering backend to use.
         */
        explicit DocraftDocumentContext(const std::shared_ptr<backend::IDocraftRenderingBackend>& backend);
        /**
         * @brief Releases context resources.
         */
        ~DocraftDocumentContext();

        /**
         * @brief Returns the active rendering backend.
         * @return Shared pointer to the rendering backend.
         */
        DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER(backend::IDocraftRenderingBackend, rendering_backend, backend_)
        /**
         * @brief Returns the mutable layout cursor.
         * @return Reference to the cursor.
         */
        DocraftCursor& cursor();
        /**
         * @brief Returns remaining vertical space on the current page section.
         * @return Available vertical space in points.
         */
        float available_space() const;
        /**
         * @brief Sets the renderer responsible for translating nodes to backend calls.
         * @param renderer Renderer instance.
         */
        void set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer);
        /**
         * @brief Returns the current renderer.
         * @return Shared pointer to the renderer (may be nullptr).
         */
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer();

        /**
         * @brief Sets the width of the current layout rectangle.
         * @param x Width in points.
         */
        void set_current_rect_width(float x);
        /**
         * @brief Returns the page width in points.
         * @return Page width in points.
         */
        [[nodiscard]] float page_width() const;
        /**
         * @brief Returns the page height in points.
         * @return Page height in points.
         */
        [[nodiscard]] float page_height() const;
        /**
         * @brief Sets the document header node.
         * @param header Header node.
         */
        void set_header(const std::shared_ptr<model::DocraftHeader> &header);
        /**
         * @brief Returns the header node.
         * @return Header node (may be nullptr).
         */
        DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER(model::DocraftHeader, header, header_)
        /**
         * @brief Sets the document body node.
         * @param body Body node.
         */
        void set_body(const std::shared_ptr<model::DocraftBody> &body);
        /**
         * @brief Returns the body node.
         * @return Body node (may be nullptr).
         */
        DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER(model::DocraftBody, body, body_)
        /**
         * @brief Sets the document footer node.
         * @param footer Footer node.
         */
        void set_footer(const std::shared_ptr<model::DocraftFooter> &footer);
        /**
         * @brief Returns the footer node.
         * @return Footer node (may be nullptr).
         */
        DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER(model::DocraftFooter, footer, footer_)
        /**
         * @brief Sets the font applier used for text nodes.
         * @param font_applier Font applier instance.
         */
        void set_font_applier(const std::shared_ptr<docraft::generic::DocraftFontApplier>& font_applier);
        /**
         * @brief Returns the font applier instance.
         * @return Font applier (may be nullptr).
         */
        DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER(docraft::generic::DocraftFontApplier, font_applier, font_applier_)
        /**
         * @brief Returns the line backend (cached).
         * @return Line rendering backend.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftLineRenderingBackend> line_backend() const;
        [[nodiscard]] std::shared_ptr<backend::IDocraftLineRenderingBackend> edit_line_backend();
        /**
         * @brief Returns the shape backend (cached).
         * @return Shape rendering backend.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftShapeRenderingBackend> shape_backend() const;
        [[nodiscard]] std::shared_ptr<backend::IDocraftShapeRenderingBackend> edit_shape_backend();
        /**
         * @brief Returns the text backend (cached).
         * @return Text rendering backend.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftTextRenderingBackend> text_backend() const;
        [[nodiscard]] std::shared_ptr<backend::IDocraftTextRenderingBackend> edit_text_backend();
        /**
         * @brief Returns the image backend (cached).
         * @return Image rendering backend.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftImageRenderingBackend> image_backend() const;
        [[nodiscard]] std::shared_ptr<backend::IDocraftImageRenderingBackend> edit_image_backend();
        /**
         * @brief Returns the page backend (cached).
         * @return Page rendering backend.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftPageRenderingBackend> page_backend() const;
        [[nodiscard]] std::shared_ptr<backend::IDocraftPageRenderingBackend> edit_page_backend();
        /**
         * @brief Replaces the underlying rendering backend.
         * @param backend New rendering backend. Pass nullptr to restore the default backend.
         */
        void set_backend(const std::shared_ptr<backend::IDocraftRenderingBackend>& backend);
        /**
         * @brief Sets the page format for the backend and updates cached size.
         */
        void set_page_format(model::DocraftPageSize size, model::DocraftPageOrientation orientation);
        /**
         * @brief Moves to the first page (index 0).
         */
        void go_to_first_page();
        /**
         * @brief Moves to the previous page.
         */
        void go_to_previous_page();
        /**
         * @brief Moves to the last page.
         */
        void go_to_last_page();
        /**
         * @brief Sets header/body/footer ratios.
         */
        void set_section_ratios(float header_ratio, float body_ratio, float footer_ratio);
        /**
         * @brief Returns the header ratio.
         */
        [[nodiscard]] float header_ratio() const;
        /**
         * @brief Returns the body ratio.
         */
        [[nodiscard]] float body_ratio() const;
        /**
         * @brief Returns the footer ratio.
         */
        [[nodiscard]] float footer_ratio() const;

    private:
        DocraftCursor cursor_;
        float current_rect_width_=0;
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer_;
        float page_width_;
        float page_height_;
        std::shared_ptr<model::DocraftHeader> header_;
        std::shared_ptr<model::DocraftBody> body_;
        std::shared_ptr<model::DocraftFooter> footer_;
        std::shared_ptr<docraft::generic::DocraftFontApplier> font_applier_;
        std::shared_ptr<backend::IDocraftRenderingBackend> backend_;
        mutable std::shared_ptr<backend::IDocraftLineRenderingBackend> line_backend_;
        mutable std::shared_ptr<backend::IDocraftShapeRenderingBackend> shape_backend_;
        mutable std::shared_ptr<backend::IDocraftTextRenderingBackend> text_backend_;
        mutable std::shared_ptr<backend::IDocraftImageRenderingBackend> image_backend_;
        mutable std::shared_ptr<backend::IDocraftPageRenderingBackend> page_backend_;
        float header_ratio_ = 0.06F;
        float body_ratio_ = 0.88F;
        float footer_ratio_ = 0.06F;
    };
} // docraft

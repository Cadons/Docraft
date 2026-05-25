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
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const backend::IDocraftRenderingBackend>,
            std::shared_ptr<backend::IDocraftRenderingBackend>,
            rendering_backend,
            edit_rendering_backend,
            backend_,
            backend_,
            DOCRAFT_ACCESSOR_INIT_NONE);
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
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const model::DocraftHeader>,
            std::shared_ptr<model::DocraftHeader>,
            header,
            edit_header,
            header_,
            header_,
            DOCRAFT_ACCESSOR_INIT_NONE);
        /**
         * @brief Sets the document body node.
         * @param body Body node.
         */
        void set_body(const std::shared_ptr<model::DocraftBody> &body);
        /**
         * @brief Returns the body node.
         * @return Body node (may be nullptr).
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const model::DocraftBody>,
            std::shared_ptr<model::DocraftBody>,
            body,
            edit_body,
            body_,
            body_,
            DOCRAFT_ACCESSOR_INIT_NONE);
        /**
         * @brief Sets the document footer node.
         * @param footer Footer node.
         */
        void set_footer(const std::shared_ptr<model::DocraftFooter> &footer);
        /**
         * @brief Returns the footer node.
         * @return Footer node (may be nullptr).
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const model::DocraftFooter>,
            std::shared_ptr<model::DocraftFooter>,
            footer,
            edit_footer,
            footer_,
            footer_,
            DOCRAFT_ACCESSOR_INIT_NONE);
        /**
         * @brief Sets the font applier used for text nodes.
         * @param font_applier Font applier instance.
         */
        void set_font_applier(const std::shared_ptr<docraft::generic::DocraftFontApplier>& font_applier);
        /**
         * @brief Returns the font applier instance.
         * @return Font applier (may be nullptr).
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const docraft::generic::DocraftFontApplier>,
            std::shared_ptr<docraft::generic::DocraftFontApplier>,
            font_applier,
            edit_font_applier,
            font_applier_,
            font_applier_,
            DOCRAFT_ACCESSOR_INIT_NONE);
        /**
         * @brief Returns the line backend (cached).
         * @return Line rendering backend.
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const backend::IDocraftLineRenderingBackend>,
            std::shared_ptr<backend::IDocraftLineRenderingBackend>,
            line_backend,
            edit_line_backend,
            line_backend_,
            line_backend_,
            docraft::ensure_lazy_backend<backend::IDocraftLineRenderingBackend>(line_backend_, backend_));
        /**
         * @brief Returns the shape backend (cached).
         * @return Shape rendering backend.
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const backend::IDocraftShapeRenderingBackend>,
            std::shared_ptr<backend::IDocraftShapeRenderingBackend>,
            shape_backend,
            edit_shape_backend,
            shape_backend_,
            shape_backend_,
            docraft::ensure_lazy_backend<backend::IDocraftShapeRenderingBackend>(shape_backend_, backend_));
        /**
         * @brief Returns the text backend (cached).
         * @return Text rendering backend.
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const backend::IDocraftTextRenderingBackend>,
            std::shared_ptr<backend::IDocraftTextRenderingBackend>,
            text_backend,
            edit_text_backend,
            text_backend_,
            text_backend_,
            docraft::ensure_lazy_backend<backend::IDocraftTextRenderingBackend>(text_backend_, backend_));
        /**
         * @brief Returns the image backend (cached).
         * @return Image rendering backend.
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const backend::IDocraftImageRenderingBackend>,
            std::shared_ptr<backend::IDocraftImageRenderingBackend>,
            image_backend,
            edit_image_backend,
            image_backend_,
            image_backend_,
            docraft::ensure_lazy_backend<backend::IDocraftImageRenderingBackend>(image_backend_, backend_));
        /**
         * @brief Returns the page backend (cached).
         * @return Page rendering backend.
         */
        DOCRAFT_CREATE_ACCESSOR_METHOD(
            std::shared_ptr<const backend::IDocraftPageRenderingBackend>,
            std::shared_ptr<backend::IDocraftPageRenderingBackend>,
            page_backend,
            edit_page_backend,
            page_backend_,
            page_backend_,
            docraft::ensure_lazy_backend<backend::IDocraftPageRenderingBackend>(page_backend_, backend_));
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

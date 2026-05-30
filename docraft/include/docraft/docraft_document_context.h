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
#include "docraft/management/docraft_backend_cache.h"
#include "docraft/management/docraft_document_section_manager.h"

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
     * The context holds the active rendering backend, page metrics, cursors, and delegates
     * section management and backend caching to specialized helper classes.
     */
    class DOCRAFT_LIB DocraftDocumentContext {
    public:
        /**
         * @brief Constructs a context with a default backend.
         */
        DocraftDocumentContext();

        /**
         * @brief Constructs a context with the provided backend.
         * @param backend Backend to use.
         */
        explicit DocraftDocumentContext(const std::shared_ptr<backend::IDocraftBackend> &backend);

        /**
         * @brief Releases context resources.
         */
        ~DocraftDocumentContext();

        /**
         * @brief Returns the active backend.
         * @return Shared pointer to the backend.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftBackend> rendering_backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftBackend> edit_rendering_backend();

        /**
         * @brief Returns the layout cursor.
         * @return Reference to the cursor.
         */
        DocraftCursor &cursor();

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
         * @brief Returns the font applier instance.
         * @return Font applier (may be nullptr).
         */
        [[nodiscard]] std::shared_ptr<const docraft::generic::DocraftFontApplier> font_applier() const;

        [[nodiscard]] std::shared_ptr<docraft::generic::DocraftFontApplier> edit_font_applier();

        /**
         * @brief Sets the font applier used for text nodes.
         * @param font_applier Font applier instance.
         */
        void set_font_applier(const std::shared_ptr<docraft::generic::DocraftFontApplier> &font_applier);

        /**
         * @brief Replaces the underlying backend.
         * @param backend New backend. Pass nullptr to restore the default backend.
         */
        void set_backend(const std::shared_ptr<backend::IDocraftBackend> &backend);

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
         * @brief Returns the section manager for header/body/footer.
         */
        management::DocraftDocumentSectionManager &section_manager();

        [[nodiscard]] const management::DocraftDocumentSectionManager &section_manager() const;

        /**
         * @brief Returns the backend cache manager.
         */
        management::DocraftBackendCache &backend_cache();

        [[nodiscard]] const management::DocraftBackendCache &backend_cache() const;

        // Backward compatibility: delegate to backend_cache()
        [[nodiscard]] std::shared_ptr<const backend::IDocraftLineRenderingBackend> line_backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftLineRenderingBackend> edit_line_backend();

        [[nodiscard]] std::shared_ptr<const backend::IDocraftShapeRenderingBackend> shape_backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftShapeRenderingBackend> edit_shape_backend();

        [[nodiscard]] std::shared_ptr<const backend::IDocraftTextRenderingBackend> text_backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftTextRenderingBackend> edit_text_backend();

        [[nodiscard]] std::shared_ptr<const backend::IDocraftImageRenderingBackend> image_backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftImageRenderingBackend> edit_image_backend();

        [[nodiscard]] std::shared_ptr<const backend::IDocraftPageRenderingBackend> page_backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftPageRenderingBackend> edit_page_backend();

        // Backward compatibility: delegate to section_manager()
        void set_header(const std::shared_ptr<model::DocraftHeader> &header);

        [[nodiscard]] std::shared_ptr<const model::DocraftHeader> header() const;

        [[nodiscard]] std::shared_ptr<model::DocraftHeader> edit_header();

        void set_body(const std::shared_ptr<model::DocraftBody> &body);

        [[nodiscard]] std::shared_ptr<const model::DocraftBody> body() const;

        [[nodiscard]] std::shared_ptr<model::DocraftBody> edit_body();

        void set_footer(const std::shared_ptr<model::DocraftFooter> &footer);

        [[nodiscard]] std::shared_ptr<const model::DocraftFooter> footer() const;

        [[nodiscard]] std::shared_ptr<model::DocraftFooter> edit_footer();

        void set_section_ratios(float header_ratio, float body_ratio, float footer_ratio);

        [[nodiscard]] float header_ratio() const;

        [[nodiscard]] float body_ratio() const;

        [[nodiscard]] float footer_ratio() const;

    private:
        /**
         * @brief Refreshes all backend caches (called after backend changes).
         */
        void refresh_backend_caches();

        DocraftCursor cursor_;
        float current_rect_width_ = 0;
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer_;
        float page_width_;
        float page_height_;
        std::shared_ptr<docraft::generic::DocraftFontApplier> font_applier_;
        std::shared_ptr<backend::IDocraftBackend> backend_;
        management::DocraftDocumentSectionManager section_manager_;
        management::DocraftBackendCache backend_cache_;
    };
} // docraft

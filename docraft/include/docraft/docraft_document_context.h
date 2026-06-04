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
#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/model/docraft_page_format.h"
#include "docraft/services/docraft_rendering_service.h"
#include "docraft/services/docraft_layout_service.h"
#include "docraft/services/docraft_typography_service.h"
#include "docraft/services/docraft_navigation_service.h"
#include <memory>

namespace docraft {
    namespace renderer {
        class DocraftAbstractRenderer;
    }

    /**
     * @brief Context Facade: Orchestrates document rendering and layout services.
     *
     * This is the primary entry point for the document rendering pipeline.
     * It composes four services:
     * - RenderingService: backend + capability caching
     * - LayoutService: cursor + page metrics
     * - TypographyService: font management
     * - NavigationService: document structure (header/body/footer) + page nav
     *
     * Responsibilities:
     * - Wiring and lifetime management of services
     * - Renderer setup and delegation
     * - Direct access to four service bundles
     */
    class DOCRAFT_LIB DocraftDocumentContext {
    public:
        /**
         * @brief Constructs a context with a default PDF backend.
         */
        DocraftDocumentContext();

        /**
         * @brief Constructs a context with backend providers factory.
         */
        DocraftDocumentContext(
            const std::shared_ptr<backend::IDocraftBackendProvidersFactory> &backend_providers_factory);

        /**
         * @brief Destructor.
         */
        ~DocraftDocumentContext();

        // ===== Service Accessors =====

        /**
         * @brief Returns the rendering service (backend + capability caching).
         */
        services::RenderingService &edit_rendering();

        [[nodiscard]] const services::RenderingService &rendering() const;

        /**
         * @brief Returns the layout service (cursor + page metrics).
         */
        services::LayoutService &edit_layout();

        [[nodiscard]] const services::LayoutService &layout() const;

        /**
         * @brief Returns the typography service (font management).
         */
        services::TypographyService &edit_typography();

        [[nodiscard]] const services::TypographyService &typography() const;

        /**
         * @brief Returns the navigation service (sections + page navigation).
         */
        services::NavigationService &edit_navigation();

        [[nodiscard]] const services::NavigationService &navigation() const;

        // ===== Renderer Management =====

        /**
         * @brief Sets the renderer responsible for translating document nodes to backend calls.
         * @param renderer Renderer instance.
         */
        void set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer);

        /**
         * @brief Returns the current renderer.
         * @return Shared pointer to the renderer (may be nullptr).
         */
        [[nodiscard]] std::shared_ptr<renderer::DocraftAbstractRenderer> renderer() const;

        [[nodiscard]] std::shared_ptr<renderer::DocraftAbstractRenderer> edit_renderer();

    private:
        void sync_layout_page_dimensions_from_backend();

        std::unique_ptr<services::RenderingService> rendering_;
        std::unique_ptr<services::LayoutService> layout_;
        std::unique_ptr<services::TypographyService> typography_;
        std::unique_ptr<services::NavigationService> navigation_;
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer_;
    };
} // namespace docraft

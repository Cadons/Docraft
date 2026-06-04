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

#include "docraft/docraft_document_context.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/renderer/docraft_renderer.h"

namespace docraft {
    DocraftDocumentContext::DocraftDocumentContext() {
        rendering_ = std::make_unique<services::RenderingService>();
        layout_ = std::make_unique<services::LayoutService>();
        typography_ = std::make_unique<services::TypographyService>();
        navigation_ = std::make_unique<services::NavigationService>();
        sync_layout_page_dimensions_from_backend();
    }

    DocraftDocumentContext::DocraftDocumentContext(
        const std::shared_ptr<backend::IDocraftCapabilityProvidersFactory> &capability_providers_factory) {
        rendering_ = std::make_unique<services::RenderingService>(capability_providers_factory);
        layout_ = std::make_unique<services::LayoutService>();
        typography_ = std::make_unique<services::TypographyService>();
        navigation_ = std::make_unique<services::NavigationService>();
        sync_layout_page_dimensions_from_backend();
    }

    DocraftDocumentContext::~DocraftDocumentContext() = default;

    services::RenderingService &DocraftDocumentContext::edit_rendering() {
        return *rendering_;
    }

    const services::RenderingService &DocraftDocumentContext::rendering() const {
        return *rendering_;
    }

    services::LayoutService &DocraftDocumentContext::edit_layout() {
        return *layout_;
    }

    const services::LayoutService &DocraftDocumentContext::layout() const {
        return *layout_;
    }

    services::TypographyService &DocraftDocumentContext::edit_typography() {
        return *typography_;
    }

    const services::TypographyService &DocraftDocumentContext::typography() const {
        return *typography_;
    }

    services::NavigationService &DocraftDocumentContext::edit_navigation() {
        return *navigation_;
    }

    const services::NavigationService &DocraftDocumentContext::navigation() const {
        return *navigation_;
    }

    void DocraftDocumentContext::set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer) {
        renderer_ = renderer;
    }

    std::shared_ptr<renderer::DocraftAbstractRenderer> DocraftDocumentContext::renderer() const {
        if (!renderer_) {
            throw docraft::exception::DocumentStateException("Renderer not set in DocraftDocumentContext");
        }
        return renderer_;
    }

    std::shared_ptr<renderer::DocraftAbstractRenderer> DocraftDocumentContext::edit_renderer() {
        return renderer_;
    }

    void DocraftDocumentContext::sync_layout_page_dimensions_from_backend() {
        auto &rendering_service = *rendering_;
        auto &layout_service = *layout_;
        if (const auto page_backend = rendering_service.page_rendering()) {
            layout_service.set_page_dimensions(page_backend->page_width(), page_backend->page_height());
        }
    }
} // namespace docraft

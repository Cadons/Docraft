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

#include "docraft/services/docraft_rendering_service.h"
#include "docraft/backend/pdf/docraft_haru_backend_providers_factory.h"

namespace docraft::services {
    namespace {
        template<typename CapabilityInterface, typename ProviderInterface>
        std::shared_ptr<const CapabilityInterface> from_const_provider(
            const std::shared_ptr<ProviderInterface> &provider,
            const CapabilityInterface *(ProviderInterface::*getter)() const) {
            if (!provider) {
                return {};
            }
            if (const auto *capability = (provider.get()->*getter)()) {
                return std::shared_ptr<const CapabilityInterface>(provider, capability);
            }
            return {};
        }

        template<typename CapabilityInterface, typename ProviderInterface>
        std::shared_ptr<CapabilityInterface> from_mutable_provider(
            const std::shared_ptr<ProviderInterface> &provider,
            CapabilityInterface *(ProviderInterface::*getter)()) {
            if (!provider) {
                return {};
            }
            if (auto *capability = (provider.get()->*getter)()) {
                return std::shared_ptr<CapabilityInterface>(provider, capability);
            }
            return {};
        }
    } // namespace

    RenderingService::RenderingService(
        const std::shared_ptr<backend::IDocraftBackendProvidersFactory> &backend_providers_factory)
        : backend_providers_factory_(backend_providers_factory
                                         ? backend_providers_factory
                                         : std::make_shared<backend::pdf::DocraftHaruBackendProvidersFactory>()) {
        backend_providers_ = backend_providers_factory_->create_backend_providers();
    }

    RenderingService::RenderingService()
        : RenderingService(std::make_shared<backend::pdf::DocraftHaruBackendProvidersFactory>()) {
    }

    RenderingService::~RenderingService() = default;

    void RenderingService::set_backend_providers_factory(
        const std::shared_ptr<backend::IDocraftBackendProvidersFactory> &backend_providers_factory) {
        backend_providers_factory_ = backend_providers_factory
                                         ? backend_providers_factory
                                         : std::make_shared<backend::pdf::DocraftHaruBackendProvidersFactory>();
        backend_providers_ = backend_providers_factory_->create_backend_providers();
    }

    std::shared_ptr<const backend::IDocraftLineRenderingBackend> RenderingService::line_rendering() const {
        return from_const_provider<backend::IDocraftLineRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::line_rendering);
    }

    std::shared_ptr<backend::IDocraftLineRenderingBackend> RenderingService::edit_line_rendering() {
        return from_mutable_provider<backend::IDocraftLineRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_line_rendering);
    }

    std::shared_ptr<const backend::IDocraftTextRenderingBackend> RenderingService::text_rendering() const {
        return from_const_provider<backend::IDocraftTextRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::text_rendering);
    }

    std::shared_ptr<backend::IDocraftTextRenderingBackend> RenderingService::edit_text_rendering() {
        return from_mutable_provider<backend::IDocraftTextRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_text_rendering);
    }

    std::shared_ptr<const backend::IDocraftShapeRenderingBackend> RenderingService::shape_rendering() const {
        return from_const_provider<backend::IDocraftShapeRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::shape_rendering);
    }

    std::shared_ptr<backend::IDocraftShapeRenderingBackend> RenderingService::edit_shape_rendering() {
        return from_mutable_provider<backend::IDocraftShapeRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_shape_rendering);
    }

    std::shared_ptr<const backend::IDocraftImageRenderingBackend> RenderingService::image_rendering() const {
        return from_const_provider<backend::IDocraftImageRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::image_rendering);
    }

    std::shared_ptr<backend::IDocraftImageRenderingBackend> RenderingService::edit_image_rendering() {
        return from_mutable_provider<backend::IDocraftImageRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_image_rendering);
    }

    std::shared_ptr<const backend::IDocraftPageRenderingBackend> RenderingService::page_rendering() const {
        return from_const_provider<backend::IDocraftPageRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::page_rendering);
    }

    std::shared_ptr<backend::IDocraftPageRenderingBackend> RenderingService::edit_page_rendering() {
        return from_mutable_provider<backend::IDocraftPageRenderingBackend>(
            backend_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_page_rendering);
    }

    std::shared_ptr<const backend::IDocraftFontBackend> RenderingService::font_backend() const {
        return from_const_provider<backend::IDocraftFontBackend>(
            backend_providers_.resource_provider,
            &backend::IDocraftResourceCapabilityProvider::font_backend);
    }

    std::shared_ptr<const backend::IDocraftOutputBackend> RenderingService::output_backend() const {
        return from_const_provider<backend::IDocraftOutputBackend>(
            backend_providers_.lifecycle_provider,
            &backend::IDocraftLifecycleCapabilityProvider::output_backend);
    }

    std::shared_ptr<const backend::IDocraftMetadataBackend> RenderingService::metadata_backend() const {
        return from_const_provider<backend::IDocraftMetadataBackend>(
            backend_providers_.lifecycle_provider,
            &backend::IDocraftLifecycleCapabilityProvider::metadata_backend);
    }

    std::shared_ptr<backend::IDocraftMetadataBackend> RenderingService::edit_metadata_backend() {
        return from_mutable_provider<backend::IDocraftMetadataBackend>(
            backend_providers_.lifecycle_provider,
            &backend::IDocraftLifecycleCapabilityProvider::edit_metadata_backend);
    }
} // namespace docraft::services



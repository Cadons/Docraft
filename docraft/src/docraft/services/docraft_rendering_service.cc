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
#include "docraft/utils/docraft_logger.h"

#include <stdexcept>

#include "docraft/exception/docraft_backend_exceptions.h"

namespace docraft::services {
    using docraft::exception::CapabilityUnavailableException;

    namespace {
        std::shared_ptr<backend::IDocraftCapabilityProvidersFactory> default_capability_factory() {
            return std::make_shared<backend::pdf::DocraftHaruCapabilityProvidersFactory>();
        }

        void handle_missing_capability(const std::string &capability_name, MissingCapabilityPolicy policy) {
            const std::string message = "Required capability not available: " + capability_name;
            switch (policy) {
                case MissingCapabilityPolicy::kFail:
                    throw CapabilityUnavailableException(message);
                case MissingCapabilityPolicy::kWarn:
                    LOG_WARNING(message);
                    return;
                case MissingCapabilityPolicy::kIgnore:
                    return;
            }
        }

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
        const std::shared_ptr<backend::IDocraftCapabilityProvidersFactory> &capability_providers_factory)
        : capability_providers_factory_(capability_providers_factory
                                            ? capability_providers_factory
                                            : default_capability_factory()) {
        capability_providers_ = capability_providers_factory_->create_capability_providers();
    }

    RenderingService::RenderingService()
        : RenderingService(default_capability_factory()) {
    }

    RenderingService::~RenderingService() = default;

    void RenderingService::set_capability_providers_factory(
        const std::shared_ptr<backend::IDocraftCapabilityProvidersFactory> &capability_providers_factory) {
        capability_providers_factory_ = capability_providers_factory
                                            ? capability_providers_factory
                                            : default_capability_factory();
        capability_providers_ = capability_providers_factory_->create_capability_providers();
    }

    void RenderingService::set_backend_providers_factory(
        const std::shared_ptr<backend::IDocraftBackendProvidersFactory> &backend_providers_factory) {
        set_capability_providers_factory(backend_providers_factory);
    }

    void RenderingService::set_missing_capability_policy(MissingCapabilityPolicy policy) {
        missing_capability_policy_ = policy;
    }

    MissingCapabilityPolicy RenderingService::missing_capability_policy() const {
        return missing_capability_policy_;
    }

    void RenderingService::validate_capabilities(const CapabilityRequirements &requirements) const {
        if (requirements.line_rendering && !line_rendering()) {
            handle_missing_capability("line_rendering", missing_capability_policy_);
        }
        if (requirements.text_rendering && !text_rendering()) {
            handle_missing_capability("text_rendering", missing_capability_policy_);
        }
        if (requirements.shape_rendering && !shape_rendering()) {
            handle_missing_capability("shape_rendering", missing_capability_policy_);
        }
        if (requirements.image_rendering && !image_rendering()) {
            handle_missing_capability("image_rendering", missing_capability_policy_);
        }
        if (requirements.page_rendering && !page_rendering()) {
            handle_missing_capability("page_rendering", missing_capability_policy_);
        }
        if (requirements.font_backend && !font_backend()) {
            handle_missing_capability("font_backend", missing_capability_policy_);
        }
        if (requirements.output_backend && !output_backend()) {
            handle_missing_capability("output_backend", missing_capability_policy_);
        }
        if (requirements.metadata_backend && !metadata_backend()) {
            handle_missing_capability("metadata_backend", missing_capability_policy_);
        }
    }

    std::shared_ptr<const backend::IDocraftLineRenderingBackend> RenderingService::line_rendering() const {
        return from_const_provider<backend::IDocraftLineRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::line_rendering);
    }

    std::shared_ptr<backend::IDocraftLineRenderingBackend> RenderingService::edit_line_rendering() {
        return from_mutable_provider<backend::IDocraftLineRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_line_rendering);
    }

    std::shared_ptr<const backend::IDocraftTextRenderingBackend> RenderingService::text_rendering() const {
        return from_const_provider<backend::IDocraftTextRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::text_rendering);
    }

    std::shared_ptr<backend::IDocraftTextRenderingBackend> RenderingService::edit_text_rendering() {
        return from_mutable_provider<backend::IDocraftTextRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_text_rendering);
    }

    std::shared_ptr<const backend::IDocraftShapeRenderingBackend> RenderingService::shape_rendering() const {
        return from_const_provider<backend::IDocraftShapeRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::shape_rendering);
    }

    std::shared_ptr<backend::IDocraftShapeRenderingBackend> RenderingService::edit_shape_rendering() {
        return from_mutable_provider<backend::IDocraftShapeRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_shape_rendering);
    }

    std::shared_ptr<const backend::IDocraftImageRenderingBackend> RenderingService::image_rendering() const {
        return from_const_provider<backend::IDocraftImageRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::image_rendering);
    }

    std::shared_ptr<backend::IDocraftImageRenderingBackend> RenderingService::edit_image_rendering() {
        return from_mutable_provider<backend::IDocraftImageRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_image_rendering);
    }

    std::shared_ptr<const backend::IDocraftPageRenderingBackend> RenderingService::page_rendering() const {
        return from_const_provider<backend::IDocraftPageRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::page_rendering);
    }

    std::shared_ptr<backend::IDocraftPageRenderingBackend> RenderingService::edit_page_rendering() {
        return from_mutable_provider<backend::IDocraftPageRenderingBackend>(
            capability_providers_.rendering_provider,
            &backend::IDocraftRenderingCapabilityProvider::edit_page_rendering);
    }

    std::shared_ptr<const backend::IDocraftFontBackend> RenderingService::font_backend() const {
        return from_const_provider<backend::IDocraftFontBackend>(
            capability_providers_.resource_provider,
            &backend::IDocraftResourceCapabilityProvider::font_backend);
    }

    std::shared_ptr<const backend::IDocraftOutputBackend> RenderingService::output_backend() const {
        return from_const_provider<backend::IDocraftOutputBackend>(
            capability_providers_.lifecycle_provider,
            &backend::IDocraftLifecycleCapabilityProvider::output_backend);
    }

    std::shared_ptr<const backend::IDocraftMetadataBackend> RenderingService::metadata_backend() const {
        return from_const_provider<backend::IDocraftMetadataBackend>(
            capability_providers_.lifecycle_provider,
            &backend::IDocraftLifecycleCapabilityProvider::metadata_backend);
    }

    std::shared_ptr<backend::IDocraftMetadataBackend> RenderingService::edit_metadata_backend() {
        return from_mutable_provider<backend::IDocraftMetadataBackend>(
            capability_providers_.lifecycle_provider,
            &backend::IDocraftLifecycleCapabilityProvider::edit_metadata_backend);
    }
} // namespace docraft::services



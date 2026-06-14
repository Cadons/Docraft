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
#include "docraft/backend/docraft_backend_providers_factory.h"
#include "docraft/backend/docraft_rendering_backend.h"
#include <memory>

namespace docraft::services {
    enum class MissingCapabilityPolicy {
        kFail,
        kWarn,
        kIgnore
    };

    struct CapabilityRequirements {
        bool line_rendering = false;
        bool text_rendering = false;
        bool shape_rendering = false;
        bool image_rendering = false;
        bool page_rendering = false;
        bool font_backend = false;
        bool output_backend = false;
        bool metadata_backend = false;
    };

    /**
     * @brief Manages the rendering backend and capability caching.
     *
     * Responsible for:
     * - Holding the active rendering backend
     * - Caching backend capabilities
     * - Providing typed access to backend capabilities
     */
    class DOCRAFT_LIB RenderingService {
    public:
        explicit RenderingService(
            const std::shared_ptr<backend::IDocraftCapabilityProvidersFactory> &capability_providers_factory);

        RenderingService();

        ~RenderingService();

        /**
         * @brief Sets backend providers factory.
         */
        void set_capability_providers_factory(
            const std::shared_ptr<backend::IDocraftCapabilityProvidersFactory> &capability_providers_factory);

        // Backward-compatible API.
        void set_backend_providers_factory(
            const std::shared_ptr<backend::IDocraftBackendProvidersFactory> &backend_providers_factory);

        void set_missing_capability_policy(MissingCapabilityPolicy policy);

        [[nodiscard]] MissingCapabilityPolicy missing_capability_policy() const;

        void validate_capabilities(const CapabilityRequirements &requirements) const;

        /**
         * @brief Returns cached line rendering capability.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftLineRenderingBackend> line_rendering() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftLineRenderingBackend> edit_line_rendering();

        /**
         * @brief Returns cached text rendering capability.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftTextRenderingBackend> text_rendering() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftTextRenderingBackend> edit_text_rendering();

        /**
         * @brief Returns cached shape rendering capability.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftShapeRenderingBackend> shape_rendering() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftShapeRenderingBackend> edit_shape_rendering();

        /**
         * @brief Returns cached image rendering capability.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftImageRenderingBackend> image_rendering() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftImageRenderingBackend> edit_image_rendering();

        /**
         * @brief Returns cached page rendering capability.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftPageRenderingBackend> page_rendering() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftPageRenderingBackend> edit_page_rendering();

        [[nodiscard]] std::shared_ptr<const backend::IDocraftFontBackend> font_backend() const;

        [[nodiscard]] std::shared_ptr<const backend::IDocraftOutputBackend> output_backend() const;

        [[nodiscard]] std::shared_ptr<const backend::IDocraftMetadataBackend> metadata_backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftMetadataBackend> edit_metadata_backend();

    private:
        backend::DocraftCapabilityProviders capability_providers_;
        std::shared_ptr<backend::IDocraftCapabilityProvidersFactory> capability_providers_factory_;
        MissingCapabilityPolicy missing_capability_policy_ = MissingCapabilityPolicy::kFail;
    };
} // namespace docraft::services


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
#include "docraft/management/docraft_backend_cache.h"
#include <memory>

namespace docraft::services {
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
        explicit RenderingService(const std::shared_ptr<backend::IDocraftBackend> &backend = nullptr);

        ~RenderingService();

        /**
         * @brief Returns the active rendering backend.
         */
        [[nodiscard]] std::shared_ptr<const backend::IDocraftBackend> backend() const;

        [[nodiscard]] std::shared_ptr<backend::IDocraftBackend> edit_backend();

        /**
         * @brief Sets a new backend and refreshes caches.
         */
        void set_backend(const std::shared_ptr<backend::IDocraftBackend> &backend);

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

        /**
         * @brief Returns the backend capability cache.
         */
        [[nodiscard]] management::DocraftBackendCache &cache();

        [[nodiscard]] const management::DocraftBackendCache &cache() const;

    private:
        void refresh_caches();

        std::shared_ptr<backend::IDocraftBackend> backend_;
        std::unique_ptr<management::DocraftBackendCache> cache_;
    };
} // namespace docraft::services


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

#include "docraft/backend/docraft_rendering_backend.h"

namespace docraft::management {
    /**
     * @brief Manages cached rendering backend interfaces.
     *
     * Provides access to specific backend interfaces (line, shape, text, image, page)
     * derived from the main rendering backend. These are cached for performance.
     */
    class DOCRAFT_LIB DocraftBackendCache {
    public:
        /**
         * @brief Initializes the backend cache from a main rendering backend.
         * @param backend The main rendering backend.
         */
        void initialize_from_backend(const std::shared_ptr<backend::IDocraftRenderingBackend>& backend);

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

    private:
        friend class DocraftDocumentContext;

        /**
         * @brief Refreshes all cached backend interfaces (called internally).
         * @param backend The main rendering backend.
         */
        void refresh_caches(const std::shared_ptr<backend::IDocraftRenderingBackend>& backend);

        std::shared_ptr<backend::IDocraftLineRenderingBackend> line_backend_;
        std::shared_ptr<backend::IDocraftShapeRenderingBackend> shape_backend_;
        std::shared_ptr<backend::IDocraftTextRenderingBackend> text_backend_;
        std::shared_ptr<backend::IDocraftImageRenderingBackend> image_backend_;
        std::shared_ptr<backend::IDocraftPageRenderingBackend> page_backend_;
    };
}


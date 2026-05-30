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

#include "docraft/management/docraft_backend_cache.h"
#include "docraft/docraft_lib.h"

namespace docraft::management {
    namespace {
        template <typename InterfaceType>
        std::shared_ptr<InterfaceType> alias_backend_interface(
            const std::shared_ptr<backend::IDocraftBackend> &backend,
            InterfaceType *interface_pointer) {
            if (!backend || !interface_pointer) {
                return {};
            }
            return std::shared_ptr<InterfaceType>(backend, interface_pointer);
        }
    } // namespace

    void DocraftBackendCache::initialize_from_backend(
        const std::shared_ptr<backend::IDocraftBackend> &backend) {
        refresh_caches(backend);
    }

    std::shared_ptr<const backend::IDocraftLineRenderingBackend> DocraftBackendCache::line_backend() const {
        return line_backend_;
    }

    std::shared_ptr<backend::IDocraftLineRenderingBackend> DocraftBackendCache::edit_line_backend() {
        return line_backend_;
    }

    std::shared_ptr<const backend::IDocraftShapeRenderingBackend> DocraftBackendCache::shape_backend() const {
        return shape_backend_;
    }

    std::shared_ptr<backend::IDocraftShapeRenderingBackend> DocraftBackendCache::edit_shape_backend() {
        return shape_backend_;
    }

    std::shared_ptr<const backend::IDocraftTextRenderingBackend> DocraftBackendCache::text_backend() const {
        return text_backend_;
    }

    std::shared_ptr<backend::IDocraftTextRenderingBackend> DocraftBackendCache::edit_text_backend() {
        return text_backend_;
    }

    std::shared_ptr<const backend::IDocraftImageRenderingBackend> DocraftBackendCache::image_backend() const {
        return image_backend_;
    }

    std::shared_ptr<backend::IDocraftImageRenderingBackend> DocraftBackendCache::edit_image_backend() {
        return image_backend_;
    }

    std::shared_ptr<const backend::IDocraftPageRenderingBackend> DocraftBackendCache::page_backend() const {
        return page_backend_;
    }

    std::shared_ptr<backend::IDocraftPageRenderingBackend> DocraftBackendCache::edit_page_backend() {
        return page_backend_;
    }

    void DocraftBackendCache::refresh_caches(const std::shared_ptr<backend::IDocraftBackend> &backend) {
        line_backend_ = alias_backend_interface(backend, backend ? backend->edit_line_rendering() : nullptr);
        shape_backend_ = alias_backend_interface(backend, backend ? backend->edit_shape_rendering() : nullptr);
        text_backend_ = alias_backend_interface(backend, backend ? backend->edit_text_rendering() : nullptr);
        image_backend_ = alias_backend_interface(backend, backend ? backend->edit_image_rendering() : nullptr);
        page_backend_ = alias_backend_interface(backend, backend ? backend->edit_page_rendering() : nullptr);
    }
} // docraft::management

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
#include "docraft/backend/pdf/docraft_haru_backend.h"

namespace docraft::services {
    RenderingService::RenderingService(const std::shared_ptr<backend::IDocraftBackend> &backend)
        : backend_(backend), cache_(std::make_unique<management::DocraftBackendCache>()) {
        if (!backend_) {
            backend_ = std::make_shared<backend::pdf::DocraftHaruBackend>();
        }
        refresh_caches();
    }

    RenderingService::~RenderingService() = default;

    std::shared_ptr<const backend::IDocraftBackend> RenderingService::backend() const {
        return backend_;
    }

    std::shared_ptr<backend::IDocraftBackend> RenderingService::edit_backend() {
        return backend_;
    }

    void RenderingService::set_backend(const std::shared_ptr<backend::IDocraftBackend> &backend) {
        backend_ = backend;
        if (!backend_) {
            backend_ = std::make_shared<backend::pdf::DocraftHaruBackend>();
        }
        refresh_caches();
    }

    std::shared_ptr<const backend::IDocraftLineRenderingBackend> RenderingService::line_rendering() const {
        return cache_->line_backend();
    }

    std::shared_ptr<backend::IDocraftLineRenderingBackend> RenderingService::edit_line_rendering() {
        return cache_->edit_line_backend();
    }

    std::shared_ptr<const backend::IDocraftTextRenderingBackend> RenderingService::text_rendering() const {
        return cache_->text_backend();
    }

    std::shared_ptr<backend::IDocraftTextRenderingBackend> RenderingService::edit_text_rendering() {
        return cache_->edit_text_backend();
    }

    std::shared_ptr<const backend::IDocraftShapeRenderingBackend> RenderingService::shape_rendering() const {
        return cache_->shape_backend();
    }

    std::shared_ptr<backend::IDocraftShapeRenderingBackend> RenderingService::edit_shape_rendering() {
        return cache_->edit_shape_backend();
    }

    std::shared_ptr<const backend::IDocraftImageRenderingBackend> RenderingService::image_rendering() const {
        return cache_->image_backend();
    }

    std::shared_ptr<backend::IDocraftImageRenderingBackend> RenderingService::edit_image_rendering() {
        return cache_->edit_image_backend();
    }

    std::shared_ptr<const backend::IDocraftPageRenderingBackend> RenderingService::page_rendering() const {
        return cache_->page_backend();
    }

    std::shared_ptr<backend::IDocraftPageRenderingBackend> RenderingService::edit_page_rendering() {
        return cache_->edit_page_backend();
    }

    management::DocraftBackendCache &RenderingService::cache() {
        return *cache_;
    }

    const management::DocraftBackendCache &RenderingService::cache() const {
        return *cache_;
    }

    void RenderingService::refresh_caches() {
        cache_->initialize_from_backend(backend_);
    }
} // namespace docraft::services



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

#include "docraft/services/docraft_layout_service.h"

namespace docraft::services {
    LayoutService::LayoutService()
        : cursor_(std::make_unique<DocraftCursor>()),
          page_width_(595.0F),
          page_height_(842.0F),
          current_rect_width_(595.0F) {
    }

    LayoutService::~LayoutService() = default;

    DocraftCursor &LayoutService::cursor() {
        return *cursor_;
    }

    const DocraftCursor &LayoutService::cursor() const {
        return *cursor_;
    }

    float LayoutService::page_width() const {
        return page_width_;
    }

    float LayoutService::page_height() const {
        return page_height_;
    }

    float LayoutService::available_space() const {
        // Legacy layout code uses available_space() as horizontal available width.
        return current_rect_width_ > 0.0F ? current_rect_width_ : page_width_;
    }

    void LayoutService::set_current_rect_width(float width) {
        current_rect_width_ = width;
    }

    float LayoutService::current_rect_width() const {
        return current_rect_width_;
    }

    void LayoutService::set_page_format(model::DocraftPageSize size, model::DocraftPageOrientation orientation) {
        // Placeholder: actual conversion from size/orientation enums to points
        // For now, use A4 defaults
        if (orientation == model::DocraftPageOrientation::kPortrait) {
            page_width_ = 595.0F;
            page_height_ = 842.0F;
        } else {
            page_width_ = 842.0F;
            page_height_ = 595.0F;
        }
        current_rect_width_ = page_width_;
    }

    void LayoutService::set_page_dimensions(float width, float height) {
        page_width_ = width > 0.0F ? width : page_width_;
        page_height_ = height > 0.0F ? height : page_height_;
        current_rect_width_ = page_width_;
    }
} // namespace docraft::services



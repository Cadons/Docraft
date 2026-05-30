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

#include "docraft/backend/pdf/docraft_haru_image_backend.h"
#include "docraft/backend/pdf/docraft_haru_page_backend.h"

#include <stdexcept>

#include <hpdf.h>

namespace docraft::backend::pdf {
    DocraftHaruImageBackend::DocraftHaruImageBackend(const std::shared_ptr<DocraftHaruSharedState> &state,
                                                     DocraftHaruPageBackend *page_backend)
        : state_(state), page_backend_(page_backend) {
    }

    void DocraftHaruImageBackend::draw_png_image(const std::string &path,
                                                 float x,
                                                 float y,
                                                 float width,
                                                 float height) const {
        auto *image = HPDF_LoadPngImageFromFile(state_ ? state_->pdf : nullptr, path.c_str());
        if (!image) {
            throw std::runtime_error("Failed to load PNG image: " + path);
        }
        HPDF_Page_DrawImage(page_backend_->current_page(), image, x, y, width, height);
    }

    void DocraftHaruImageBackend::draw_png_image_from_memory(const unsigned char *data,
                                                             std::size_t size,
                                                             float x,
                                                             float y,
                                                             float width,
                                                             float height) const {
        auto *image = HPDF_LoadPngImageFromMem(
            state_ ? state_->pdf : nullptr,
            reinterpret_cast<const HPDF_BYTE *>(data),
            static_cast<HPDF_UINT>(size));
        if (!image) {
            throw std::runtime_error("Failed to load PNG image from memory");
        }
        HPDF_Page_DrawImage(page_backend_->current_page(), image, x, y, width, height);
    }

    void DocraftHaruImageBackend::draw_jpeg_image(const std::string &path,
                                                  float x,
                                                  float y,
                                                  float width,
                                                  float height) const {
        auto *image = HPDF_LoadJpegImageFromFile(state_ ? state_->pdf : nullptr, path.c_str());
        if (!image) {
            throw std::runtime_error("Failed to load JPEG image: " + path);
        }
        HPDF_Page_DrawImage(page_backend_->current_page(), image, x, y, width, height);
    }

    void DocraftHaruImageBackend::draw_jpeg_image_from_memory(const unsigned char *data,
                                                              std::size_t size,
                                                              float x,
                                                              float y,
                                                              float width,
                                                              float height) const {
        auto *image = HPDF_LoadJpegImageFromMem(
            state_ ? state_->pdf : nullptr,
            reinterpret_cast<const HPDF_BYTE *>(data),
            static_cast<HPDF_UINT>(size));
        if (!image) {
            throw std::runtime_error("Failed to load JPEG image from memory");
        }
        HPDF_Page_DrawImage(page_backend_->current_page(), image, x, y, width, height);
    }

    void DocraftHaruImageBackend::draw_raw_rgb_image(const std::string &path,
                                                     int pixel_width,
                                                     int pixel_height,
                                                     float x,
                                                     float y,
                                                     float width,
                                                     float height) const {
        auto *image = HPDF_LoadRawImageFromFile(
            state_ ? state_->pdf : nullptr,
            path.c_str(),
            static_cast<HPDF_UINT>(pixel_width),
            static_cast<HPDF_UINT>(pixel_height),
            HPDF_CS_DEVICE_RGB);
        if (!image) {
            throw std::runtime_error("Failed to load raw RGB image: " + path);
        }
        HPDF_Page_DrawImage(page_backend_->current_page(), image, x, y, width, height);
    }

    void DocraftHaruImageBackend::draw_raw_rgb_image_from_memory(const unsigned char *data,
                                                                 int pixel_width,
                                                                 int pixel_height,
                                                                 float x,
                                                                 float y,
                                                                 float width,
                                                                 float height) const {
        constexpr HPDF_UINT kBitsPerComponent = 8;
        auto *image = HPDF_LoadRawImageFromMem(
            state_ ? state_->pdf : nullptr,
            reinterpret_cast<const HPDF_BYTE *>(data),
            static_cast<HPDF_UINT>(pixel_width),
            static_cast<HPDF_UINT>(pixel_height),
            HPDF_CS_DEVICE_RGB,
            kBitsPerComponent);
        if (!image) {
            throw std::runtime_error("Failed to load raw RGB image from memory");
        }
        HPDF_Page_DrawImage(page_backend_->current_page(), image, x, y, width, height);
    }
} // namespace docraft::backend::pdf


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

#include "docraft/backend/docraft_image_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"

#include <memory>

namespace docraft::backend::pdf {
    class DocraftHaruPageBackend;

    /**
     * @brief Haru implementation of image rendering operations.
     */
    class DocraftHaruImageBackend : public docraft::backend::IDocraftImageRenderingBackend {
    public:
        /**
         * @brief Creates an image backend bound to a Haru document owner.
         */
        explicit DocraftHaruImageBackend(const std::shared_ptr<DocraftHaruSharedState> &state,
                                         DocraftHaruPageBackend *page_backend);

        /**
         * @brief Loads and draws a PNG image from a file path.
         */
        void draw_png_image(const std::string &path,
                            float x,
                            float y,
                            float width,
                            float height) const override;

        /**
         * @brief Loads and draws a PNG image from an in-memory buffer.
         */
        void draw_png_image_from_memory(const unsigned char *data,
                                        std::size_t size,
                                        float x,
                                        float y,
                                        float width,
                                        float height) const override;

        /**
         * @brief Loads and draws a JPEG image from a file path.
         */
        void draw_jpeg_image(const std::string &path,
                             float x,
                             float y,
                             float width,
                             float height) const override;

        /**
         * @brief Loads and draws a JPEG image from an in-memory buffer.
         */
        void draw_jpeg_image_from_memory(const unsigned char *data,
                                         std::size_t size,
                                         float x,
                                         float y,
                                         float width,
                                         float height) const override;

        /**
         * @brief Loads and draws a raw RGB image from a file.
         */
        void draw_raw_rgb_image(const std::string &path,
                                int pixel_width,
                                int pixel_height,
                                float x,
                                float y,
                                float width,
                                float height) const override;

        /**
         * @brief Loads and draws a raw RGB image from an in-memory buffer.
         */
        void draw_raw_rgb_image_from_memory(const unsigned char *data,
                                            int pixel_width,
                                            int pixel_height,
                                            float x,
                                            float y,
                                            float width,
                                            float height) const override;

    private:
        std::shared_ptr<DocraftHaruSharedState> state_;
        DocraftHaruPageBackend *page_backend_ = nullptr;
    };
} // namespace docraft::backend::pdf


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
#include <cstddef>
#include <string>

namespace docraft::backend {
    /**
     * @brief Interface for image rendering backends used by Docraft.
     */
    class DOCRAFT_LIB IDocraftImageRenderingBackend {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IDocraftImageRenderingBackend() = default;
        /**
         * @brief Draws a PNG image from file.
         */
        virtual void draw_png_image(
            const std::string& path,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a PNG image from memory.
         */
        virtual void draw_png_image_from_memory(
            const unsigned char* data,
            std::size_t size,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a JPEG image from file.
         */
        virtual void draw_jpeg_image(
            const std::string& path,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a JPEG image from memory.
         */
        virtual void draw_jpeg_image_from_memory(
            const unsigned char* data,
            std::size_t size,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a raw RGB image from file.
         */
        virtual void draw_raw_rgb_image(
            const std::string& path,
            int pixel_width,
            int pixel_height,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a raw RGB image from memory.
         */
        virtual void draw_raw_rgb_image_from_memory(
            const unsigned char* data,
            int pixel_width,
            int pixel_height,
            float x,
            float y,
            float width,
            float height) const = 0;
    };
} // docraft::backend

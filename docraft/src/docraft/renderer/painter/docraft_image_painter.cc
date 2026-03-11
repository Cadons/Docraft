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

#include "docraft/renderer/painter/docraft_image_painter.h"

#include <stdexcept>

#include "docraft/backend/docraft_image_rendering_backend.h"

namespace docraft::renderer::painter {
    DocraftImagePainter::DocraftImagePainter(const model::DocraftImage &image_node) : image_node_(image_node) {
    }

    void DocraftImagePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) return;
        auto backend = context->image_backend();
        if (!backend) return;

        if (!image_node_.visible()) {
            return;
        }
        //start with pdf, then other formats will be supported
        switch (image_node_.format()) {
            case model::ImageFormat::kPng:
                backend->draw_png_image(
                    image_node_.path(),
                    image_node_.position().x,
                    image_node_.position().y - image_node_.height(),
                    image_node_.width(),
                    image_node_.height());
                break;
            case model::ImageFormat::kJpeg:
                backend->draw_jpeg_image(
                    image_node_.path(),
                    image_node_.position().x,
                    image_node_.position().y - image_node_.height(),
                    image_node_.width(),
                    image_node_.height());
                break;
            case model::ImageFormat::kRaw:
                if (image_node_.raw_data().empty()) {
                    throw std::runtime_error("Raw image data is empty");
                }
                backend->draw_raw_rgb_image_from_memory(
                    image_node_.raw_data().data(),
                    image_node_.raw_pixel_width(),
                    image_node_.raw_pixel_height(),
                    image_node_.position().x,
                    image_node_.position().y - image_node_.height(),
                    image_node_.width(),
                    image_node_.height());
                break;
            default:
                throw std::runtime_error("Unsupported image format");
        }
    }
} // docraft

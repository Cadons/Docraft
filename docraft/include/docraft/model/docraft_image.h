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
#include <vector>

#include "docraft/model/docraft_node.h"
#include "docraft/model/i_docraft_clonable.h"

namespace docraft::model {
        /**
         * @brief Supported image formats for DocraftImage.
         */
        enum class ImageFormat{
                kPng,
                kJpeg,
                kRaw
            };
        /**
         * @brief Image node that draws bitmap content.
         *
         * Images can be loaded from file or provided as raw pixel data.
         */
        class DOCRAFT_LIB DocraftImage : public DocraftNode, public IDocraftClonable {
        public:
                /**
                 * @brief Creates an empty image node.
                 */
                DocraftImage();
                DocraftImage(const DocraftImage &node) = default;
                ~DocraftImage() override = default;
                /**
                 * @brief Constructs from an existing node pointer (copy).
                 */
                explicit DocraftImage(DocraftImage *node);
                /**
                 * @brief Draws the image using the provided context.
                 * @param context Document context.
                 */
                void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
                /**
                 * @brief Clones the image node.
                 * @return Shared pointer to the cloned node.
                 */
                std::shared_ptr<DocraftNode> clone() const override;
                /**
                 * @brief Sets the image file path.
                 * @param path Image file path.
                 */
                void set_path(const std::string &path);
                /**
                 * @brief Sets raw image data and its pixel dimensions.
                 * @param data Raw pixel data.
                 * @param pixel_width Image width in pixels.
                 * @param pixel_height Image height in pixels.
                 */
                void set_raw_data(const std::vector<unsigned char> &data, int pixel_width, int pixel_height);

                /**
                 * @brief Sets whether the image has raw pixel data.
                 * @param has_raw_data True if raw data is set, false otherwise.
                 */
                void set_has_raw_data(bool has_raw_data);

                /**
                 * @brief Returns image dimensions from a file path.
                 * @param path Image file path.
                 * @param format Image format.
                 * @return Pair of width/height in points.
                 */
                static std::pair<float, float> get_image_dimensions(const std::string &path, ImageFormat format);

                /**
                 * @brief Returns the image format.
                 * @return Image format enum.
                 */
                [[nodiscard]] ImageFormat format() const;
                /**
                 * @brief Returns the image file path.
                 * @return Image file path.
                 */
                [[nodiscard]] const std::string &path() const;
                /**
                 * @brief Returns raw image data.
                 * @return Raw pixel buffer.
                 */
                [[nodiscard]] const std::vector<unsigned char> &raw_data() const;
                /**
                 * @brief Returns raw image pixel width.
                 * @return Pixel width.
                 */
                [[nodiscard]] int raw_pixel_width() const;
                /**
                 * @brief Returns raw image pixel height.
                 * @return Pixel height.
                 */
                [[nodiscard]] int raw_pixel_height() const;

                /**
                 * @brief Checks if the image has raw pixel data.
                 * raw data is considered set if the format is kRaw and the raw_data vector is not empty.
                 * This is used to determine if the image should be rendered from raw data instead of a file path.
                 * @return True if raw data is set, false otherwise.
                 */
                [[nodiscard]] bool has_raw_data() const;

        private:
                std::string path_;
                ImageFormat format_=ImageFormat::kPng;
                std::vector<unsigned char> raw_data_;
                int raw_pixel_width_ = 0;
                int raw_pixel_height_ = 0;
                bool has_raw_data_=false;
        };
} // docraft

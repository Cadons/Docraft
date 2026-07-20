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

#include "docraft/craft/parser/docraft_parser.h"

#include <string_view>

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/utils/docraft_base64.h"

namespace docraft::craft::parser {
    namespace {
        bool extract_base64_payload(std::string_view value, std::string_view &payload) {
            const std::string_view prefix = "base64:";
            if (value.starts_with(prefix)) {
                payload = value.substr(prefix.size());
                return true;
            }
            const std::string_view marker = "base64,";
            const auto pos = value.find(marker);
            if (pos != std::string_view::npos) {
                payload = value.substr(pos + marker.size());
                return true;
            }
            return false;
        }
    } // namespace

    std::any DocraftImageParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedImageData data;

        const bool has_src = craft_language_source.attribute(elements::image::attribute::kSrc.data()) != nullptr;
        const bool has_data = craft_language_source.attribute(elements::image::attribute::kData.data()) != nullptr;
        if (has_src && has_data)
        {
            throw docraft::exception::InvalidInputException("Image node cannot have both 'src' and 'data' attributes.");
        }

        if (auto src_attr = craft_language_source.attribute(elements::image::attribute::kSrc.data())) {
            data.path = src_attr.as_string();
        }
        if (auto raw_data_attr = craft_language_source.attribute(elements::image::attribute::kData.data())) {
            std::string_view payload;
            const std::string_view raw_value = raw_data_attr.as_string();
            if (extract_base64_payload(raw_value, payload)) {
                const auto width_attr = craft_language_source.attribute(elements::image::attribute::kDataWidth.data());
                const auto height_attr = craft_language_source.attribute(elements::image::attribute::kDataHeight.data());
                if (!width_attr || !height_attr) {
                    throw docraft::exception::InvalidInputException(
                        "Base64 image data requires data_width and data_height.");
                }
                const int pixel_width = width_attr.as_int();
                const int pixel_height = height_attr.as_int();
                if (pixel_width <= 0 || pixel_height <= 0) {
                    throw docraft::exception::InvalidInputException("Base64 image data has invalid dimensions.");
                }
                auto decoded = utils::decode_base64(payload);
                const auto expected_size = static_cast<size_t>(pixel_width) *
                                           static_cast<size_t>(pixel_height) * 3U;
                if (decoded.size() != expected_size) {
                    throw docraft::exception::InvalidInputException(
                        "Base64 image data size does not match dimensions (RGB expected).");
                }
                data.raw_data = std::move(decoded);
                data.raw_pixel_width = pixel_width;
                data.raw_pixel_height = pixel_height;
            } else {
                // Not base64: the raw value is a data-binding key, resolved at templating
                // time. Carried through as `path` (structural port only -- templating
                // resolution itself is a later phase).
                data.path = raw_data_attr.as_string();
            }
        }
        return data;
    }
} // namespace docraft::craft::parser

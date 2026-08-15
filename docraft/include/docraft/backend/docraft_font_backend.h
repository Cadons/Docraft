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
     * @brief Capability interface for backend font registration and selection.
     */
    class DOCRAFT_LIB IDocraftFontBackend {
    public:
        virtual ~IDocraftFontBackend() = default;

        /**
         * @brief Registers a TTF font and returns the internal name.
         * @param path Font file path.
         * @param embed Whether to embed the font in the document.
         * @return Backend internal font name.
         */
        virtual const char *register_ttf_font_from_file(const std::string &path, bool embed) const = 0;

        /**
         * @brief Registers a TTF font from in-memory bytes and returns the internal name.
         * @param data Raw TTF font bytes.
         * @param size Size of data in bytes.
         * @param embed Whether to embed the font in the document.
         * @return Backend internal font name, or nullptr on failure.
         */
        virtual const char *register_ttf_font_from_memory(const unsigned char *data, std::size_t size,
                                                           bool embed) const = 0;

        /**
         * @brief Checks whether a font can be used with the given encoder.
         * @param internal_name Backend internal font name.
         * @param encoder Backend encoder name.
         * @return true if the font can be used.
         */
        virtual bool can_use_font(const std::string &internal_name, const char *encoder) const = 0;

        /**
         * @brief Sets the current font and size.
         * @param internal_name Backend internal font name.
         * @param size Font size in points.
         * @param encoder Backend encoder name.
         */
        virtual void set_font(const std::string &internal_name, float size, const char *encoder) const = 0;
    };
} // namespace docraft::backend


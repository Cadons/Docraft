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
#include <string>

#include "docraft/backend/docraft_image_rendering_backend.h"
#include "docraft/backend/docraft_page_rendering_backend.h"
#include "docraft/backend/docraft_shape_rendering_backend.h"
#include "docraft/backend/docraft_text_rendering_backend.h"

namespace docraft {
    class DocraftDocumentMetadata;
}

namespace docraft::backend {
    /**
     * @brief Aggregated rendering backend interface.
     */
    class DOCRAFT_LIB IDocraftRenderingBackend : public IDocraftTextRenderingBackend,
                                     public IDocraftShapeRenderingBackend,
                                     public IDocraftImageRenderingBackend,
                                     public IDocraftPageRenderingBackend {
    public:
        /**
         * @brief Virtual destructor.
         */
        ~IDocraftRenderingBackend() override = default;
        /**
         * @brief Saves the document to a file path.
         * @param path Output file path.
         */
        virtual void save_to_file(const std::string& path) const = 0;
        /**
         * @brief Returns the preferred file extension for this backend.
         * @return Extension with or without leading dot (e.g. ".pdf" or "pdf").
         */
        [[nodiscard]] virtual std::string file_extension() const = 0;
        /**
         * @brief Registers a TTF font and returns the internal name.
         * @param path Font file path.
         * @param embed Whether to embed the font in the document.
         * @return Backend internal font name.
         */
        virtual const char* register_ttf_font_from_file(const std::string& path, bool embed) const = 0;
        /**
         * @brief Checks whether a font can be used with the given encoder.
         * @param internal_name Backend internal font name.
         * @param encoder Backend encoder name.
         * @return true if the font can be used.
         */
        virtual bool can_use_font(const std::string& internal_name, const char* encoder) const = 0;
        /**
         * @brief Sets the current font and size.
         * @param internal_name Backend internal font name.
         * @param size Font size in points.
         * @param encoder Backend encoder name.
         */
        virtual void set_font(const std::string& internal_name, float size, const char* encoder) const = 0;
        /**
         * @brief Applies document metadata to the backend document.
         * @param metadata Metadata values to apply.
         */
        virtual void set_document_metadata(const DocraftDocumentMetadata &metadata) = 0;
    };
} // docraft::backend

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

#include "docraft/backend/docraft_font_backend.h"
#include "docraft/backend/docraft_image_rendering_backend.h"
#include "docraft/backend/docraft_line_rendering_backend.h"
#include "docraft/backend/docraft_metadata_backend.h"
#include "docraft/backend/docraft_output_backend.h"
#include "docraft/backend/docraft_page_rendering_backend.h"
#include "docraft/backend/docraft_shape_rendering_backend.h"
#include "docraft/backend/docraft_text_rendering_backend.h"

namespace docraft::backend {
    /**
     * @brief Rendering capability contract (geometry, text, images, pages).
     */
    class DOCRAFT_LIB IDocraftRenderingCapabilityProvider {
    public:
        virtual ~IDocraftRenderingCapabilityProvider() = default;

        [[nodiscard]] virtual const IDocraftLineRenderingBackend *line_rendering() const = 0;

        [[nodiscard]] virtual IDocraftLineRenderingBackend *edit_line_rendering() = 0;

        [[nodiscard]] virtual const IDocraftTextRenderingBackend *text_rendering() const = 0;

        [[nodiscard]] virtual IDocraftTextRenderingBackend *edit_text_rendering() = 0;

        [[nodiscard]] virtual const IDocraftShapeRenderingBackend *shape_rendering() const = 0;

        [[nodiscard]] virtual IDocraftShapeRenderingBackend *edit_shape_rendering() = 0;

        [[nodiscard]] virtual const IDocraftImageRenderingBackend *image_rendering() const = 0;

        [[nodiscard]] virtual IDocraftImageRenderingBackend *edit_image_rendering() = 0;

        [[nodiscard]] virtual const IDocraftPageRenderingBackend *page_rendering() const = 0;

        [[nodiscard]] virtual IDocraftPageRenderingBackend *edit_page_rendering() = 0;
    };

    /**
     * @brief Resource capability contract (fonts and related resources).
     */
    class DOCRAFT_LIB IDocraftResourceCapabilityProvider {
    public:
        virtual ~IDocraftResourceCapabilityProvider() = default;

        [[nodiscard]] virtual const IDocraftFontBackend *font_backend() const = 0;

        [[nodiscard]] virtual IDocraftFontBackend *edit_font_backend() = 0;
    };

    /**
     * @brief Document lifecycle capability contract (metadata and persistence).
     */
    class DOCRAFT_LIB IDocraftLifecycleCapabilityProvider {
    public:
        virtual ~IDocraftLifecycleCapabilityProvider() = default;

        [[nodiscard]] virtual const IDocraftOutputBackend *output_backend() const = 0;

        [[nodiscard]] virtual IDocraftOutputBackend *edit_output_backend() = 0;

        [[nodiscard]] virtual const IDocraftMetadataBackend *metadata_backend() const = 0;

        [[nodiscard]] virtual IDocraftMetadataBackend *edit_metadata_backend() = 0;
    };

} // docraft::backend

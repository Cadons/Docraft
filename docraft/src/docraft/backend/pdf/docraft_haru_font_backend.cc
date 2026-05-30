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

#include "docraft/backend/pdf/docraft_haru_font_backend.h"

#include "docraft/backend/pdf/docraft_haru_page_backend.h"

#include <stdexcept>

#include <hpdf.h>

namespace docraft::backend::pdf {
    DocraftHaruBackend::FontHaruBackend::FontHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {
    }

    const char *DocraftHaruBackend::FontHaruBackend::register_ttf_font_from_file(const std::string &path,
        bool embed) const {
        const char *result = HPDF_LoadTTFontFromFile(owner_.pdf_, path.c_str(), embed ? HPDF_TRUE : HPDF_FALSE);
        if (!result) {
            HPDF_ResetError(owner_.pdf_);
        }
        return result;
    }

    bool DocraftHaruBackend::FontHaruBackend::can_use_font(const std::string &internal_name,
                                                           const char *encoder) const {
        HPDF_Font font = HPDF_GetFont(owner_.pdf_, internal_name.c_str(), encoder);
        if (!font || HPDF_GetError(owner_.pdf_) != HPDF_OK) {
            HPDF_ResetError(owner_.pdf_);
            return false;
        }
        return true;
    }

    void DocraftHaruBackend::FontHaruBackend::set_font(const std::string &internal_name,
                                                       float size,
                                                       const char *encoder) const {
        HPDF_Font font = HPDF_GetFont(owner_.pdf_, internal_name.c_str(), encoder);
        if (!font) {
            throw std::runtime_error("Failed to resolve font: " + internal_name);
        }
        HPDF_Page_SetFontAndSize(owner_.page_backend_->current_page(), font, size);
    }
} // namespace docraft::backend::pdf

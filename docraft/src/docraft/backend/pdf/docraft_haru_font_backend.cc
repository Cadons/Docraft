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
    DocraftHaruFontBackend::DocraftHaruFontBackend(const std::shared_ptr<DocraftHaruSharedState> &state,
                                                   DocraftHaruPageBackend *page_backend)
        : state_(state), page_backend_(page_backend) {
    }

    const char *DocraftHaruFontBackend::register_ttf_font_from_file(const std::string &path,
                                                                    bool embed) const {
        const auto pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            return nullptr;
        }
        const char *result = HPDF_LoadTTFontFromFile(pdf,
                                                     path.c_str(),
                                                     embed ? HPDF_TRUE : HPDF_FALSE);
        if (!result) {
            HPDF_ResetError(pdf);
        }
        return result;
    }

    bool DocraftHaruFontBackend::can_use_font(const std::string &internal_name,
                                              const char *encoder) const {
        const auto pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            return false;
        }
        HPDF_Font font = HPDF_GetFont(pdf, internal_name.c_str(), encoder);
        if (!font || HPDF_GetError(pdf) != HPDF_OK) {
            HPDF_ResetError(pdf);
            return false;
        }
        return true;
    }

    void DocraftHaruFontBackend::set_font(const std::string &internal_name,
                                          float size,
                                          const char *encoder) const {
        const auto pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            throw std::runtime_error("Haru document is not initialized");
        }
        HPDF_Font font = HPDF_GetFont(pdf, internal_name.c_str(), encoder);
        if (!font) {
            throw std::runtime_error("Failed to resolve font: " + internal_name);
        }
        HPDF_Page_SetFontAndSize(page_backend_->current_page(), font, size);
    }
} // namespace docraft::backend::pdf

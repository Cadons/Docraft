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

#include "docraft/backend/pdf/docraft_haru_text_backend.h"
#include "docraft/backend/pdf/docraft_haru_page_backend.h"

#include <hpdf.h>

namespace docraft::backend::pdf {
    DocraftHaruBackend::TextHaruBackend::TextHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {
    }

    void DocraftHaruBackend::TextHaruBackend::begin_text() const {
        HPDF_Page_BeginText(owner_.page_backend_->current_page());
    }

    void DocraftHaruBackend::TextHaruBackend::end_text() const {
        HPDF_Page_EndText(owner_.page_backend_->current_page());
    }

    void DocraftHaruBackend::TextHaruBackend::draw_text(const std::string &text, float x, float y) const {
        HPDF_Page_TextOut(owner_.page_backend_->current_page(), x, y, text.c_str());
    }

    void DocraftHaruBackend::TextHaruBackend::set_text_color(float r, float g, float b) const {
        HPDF_Page_SetRGBFill(owner_.page_backend_->current_page(), r, g, b);
    }

    void DocraftHaruBackend::TextHaruBackend::draw_text_matrix(const std::string &text,
                                                               float scale_x,
                                                               float skew_x,
                                                               float skew_y,
                                                               float scale_y,
                                                               float translate_x,
                                                               float translate_y) const {
        HPDF_Page_SetTextMatrix(
            owner_.page_backend_->current_page(),
            scale_x,
            skew_x,
            skew_y,
            scale_y,
            translate_x,
            translate_y);
        HPDF_Page_ShowText(owner_.page_backend_->current_page(), text.c_str());
    }

    float DocraftHaruBackend::TextHaruBackend::measure_text_width(const std::string &text) const {
        return HPDF_Page_TextWidth(owner_.page_backend_->current_page(), text.c_str());
    }
} // namespace docraft::backend::pdf


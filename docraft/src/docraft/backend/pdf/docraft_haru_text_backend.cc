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
    DocraftHaruTextBackend::DocraftHaruTextBackend(const std::shared_ptr<DocraftHaruSharedState> &state,
                                                   DocraftHaruPageBackend *page_backend)
        : state_(state), page_backend_(page_backend) {
    }

    void DocraftHaruTextBackend::begin_text() const {
        HPDF_Page_BeginText(page_backend_->current_page());
    }

    void DocraftHaruTextBackend::end_text() const {
        HPDF_Page_EndText(page_backend_->current_page());
    }

    void DocraftHaruTextBackend::draw_text(const std::string &text, float x, float y) const {
        HPDF_Page_TextOut(page_backend_->current_page(), x, y, text.c_str());
    }

    void DocraftHaruTextBackend::set_text_color(float r, float g, float b) const {
        HPDF_Page_SetRGBFill(page_backend_->current_page(), r, g, b);
    }

    void DocraftHaruTextBackend::draw_text_matrix(const std::string &text,
                                                  float scale_x,
                                                  float skew_x,
                                                  float skew_y,
                                                  float scale_y,
                                                  float translate_x,
                                                  float translate_y) const {
        HPDF_Page_SetTextMatrix(
            page_backend_->current_page(),
            scale_x,
            skew_x,
            skew_y,
            scale_y,
            translate_x,
            translate_y);
        HPDF_Page_ShowText(page_backend_->current_page(), text.c_str());
    }

    float DocraftHaruTextBackend::measure_text_width(const std::string &text) const {
        return HPDF_Page_TextWidth(page_backend_->current_page(), text.c_str());
    }
} // namespace docraft::backend::pdf


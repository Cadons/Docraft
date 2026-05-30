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

#include "docraft/backend/pdf/docraft_haru_shape_backend.h"
#include "docraft/backend/pdf/docraft_haru_page_backend.h"

#include <hpdf.h>

namespace docraft::backend::pdf {
    DocraftHaruShapeBackend::DocraftHaruShapeBackend(const std::shared_ptr<DocraftHaruSharedState> &state,
                                                     DocraftHaruPageBackend *page_backend)
        : state_(state), page_backend_(page_backend) {
    }

    void DocraftHaruShapeBackend::save_state() const {
        HPDF_Page_GSave(page_backend_->current_page());
    }

    void DocraftHaruShapeBackend::restore_state() const {
        HPDF_Page_GRestore(page_backend_->current_page());
    }

    void DocraftHaruShapeBackend::set_fill_color(float r, float g, float b) const {
        HPDF_Page_SetRGBFill(page_backend_->current_page(), r, g, b);
    }

    void DocraftHaruShapeBackend::set_fill_alpha(float alpha) const {
        fill_alpha_ = alpha;
        apply_alpha_state();
    }

    void DocraftHaruShapeBackend::set_stroke_alpha(float alpha) const {
        stroke_alpha_ = alpha;
        apply_alpha_state();
    }

    void DocraftHaruShapeBackend::draw_rectangle(float x, float y, float width, float height) const {
        HPDF_Page_Rectangle(page_backend_->current_page(), x, y, width, height);
    }

    void DocraftHaruShapeBackend::draw_circle(float center_x, float center_y, float radius) const {
        HPDF_Page_Circle(page_backend_->current_page(), center_x, center_y, radius);
    }

    void DocraftHaruShapeBackend::draw_polygon(const std::vector<model::DocraftPoint> &points) const {
        if (points.size() < 2U) {
            return;
        }

        HPDF_Page_MoveTo(page_backend_->current_page(), points[0].x, points[0].y);
        for (size_t i = 1; i < points.size(); ++i) {
            HPDF_Page_LineTo(page_backend_->current_page(), points[i].x, points[i].y);
        }
        HPDF_Page_ClosePath(page_backend_->current_page());
    }

    void DocraftHaruShapeBackend::fill() const {
        HPDF_Page_Fill(page_backend_->current_page());
    }

    void DocraftHaruShapeBackend::stroke() const {
        HPDF_Page_Stroke(page_backend_->current_page());
    }

    void DocraftHaruShapeBackend::fill_stroke() const {
        HPDF_Page_FillStroke(page_backend_->current_page());
    }

    void DocraftHaruShapeBackend::apply_alpha_state() const {
        auto *ext = HPDF_CreateExtGState(state_ ? state_->pdf : nullptr);
        if (ext) {
            HPDF_ExtGState_SetAlphaFill(ext, fill_alpha_);
            HPDF_ExtGState_SetAlphaStroke(ext, stroke_alpha_);
            HPDF_Page_SetExtGState(page_backend_->current_page(), ext);
        }
    }
} // namespace docraft::backend::pdf


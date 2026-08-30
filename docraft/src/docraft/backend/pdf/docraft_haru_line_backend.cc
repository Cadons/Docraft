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

#include "docraft/backend/pdf/docraft_haru_line_backend.h"

#include <hpdf.h>

#include <utility>

namespace docraft::backend::pdf {
    DocraftHaruLineBackend::DocraftHaruLineBackend(const std::shared_ptr<DocraftHaruSharedState> &state)
        : state_(state) {
    }

    void DocraftHaruLineBackend::set_stroke_color(float r, float g, float b) const {
        auto *provider = state_->ensure_page_provider();
        HPDF_Page_SetRGBStroke(provider->current_page(), r, g, b);
    }

    void DocraftHaruLineBackend::set_line_width(float thickness) const {
        auto *provider = state_->ensure_page_provider();
        HPDF_Page_SetLineWidth(provider->current_page(), thickness);
    }

    void DocraftHaruLineBackend::set_line_dash_pattern(const std::vector<float>& pattern) const {
        auto *provider = state_->ensure_page_provider();
        if (pattern.empty()) {
            HPDF_Page_SetDash(provider->current_page(), nullptr, 0, 0.0F);
            return;
        }
        HPDF_Page_SetDash(provider->current_page(), pattern.data(),
                          static_cast<HPDF_UINT>(pattern.size()), 0.0F);
    }

    void DocraftHaruLineBackend::draw_line(float x1, float y1, float x2, float y2) const {
        auto *provider = state_->ensure_page_provider();
        float px1, py1, px2, py2;
        provider->compute_coordinate_system(x1, y1, px1, py1);
        provider->compute_coordinate_system(x2, y2, px2, py2);
        HPDF_Page page = provider->current_page();
        HPDF_Page_MoveTo(page, px1, py1);
        HPDF_Page_LineTo(page, px2, py2);
        HPDF_Page_Stroke(page);
    }

    namespace {
        // Clamps a possibly out-of-range neighbor index to the valid [0, n-1] range by
        // duplicating the nearest endpoint -- the standard way to terminate a Catmull-Rom
        // spline so the first/last segment still has two well-defined neighbors to derive
        // its tangent from.
        std::size_t clamp_neighbor_index(long index, std::size_t point_count)
        {
            if (index < 0)
            {
                return 0;
            }
            if (std::cmp_greater_equal(index ,point_count))
            {
                return point_count - 1;
            }
            return static_cast<std::size_t>(index);
        }
    }

    void DocraftHaruLineBackend::draw_curve(const std::vector<Position>& points) const {
        if (points.size() < 2)
        {
            return;
        }
        auto const*provider = state_->ensure_page_provider();
        HPDF_Page page = provider->current_page();

        float px;
        float py;
        provider->compute_coordinate_system(points[0].x, points[0].y, px, py);
        HPDF_Page_MoveTo(page, px, py);

        if (points.size() == 2)
        {
            provider->compute_coordinate_system(points[1].x, points[1].y, px, py);
            HPDF_Page_LineTo(page, px, py);
            HPDF_Page_Stroke(page);
            return;
        }

        // Converts the uniform Catmull-Rom spline through `points` into a piecewise
        // cubic Bezier path: for each segment [P1,P2], the standard Catmull-Rom-to-
        // Bezier derivation places the two control points at
        // P1 + (P2-P0)/6 and P2 - (P3-P1)/6, using the segment's neighbors P0/P3 (missing
        // neighbors at either end are clamped to the nearest endpoint, see
        // clamp_neighbor_index()) to derive a tangent that keeps the curve smooth (C1
        // continuous) across every interior point, without overshooting the way a naive
        // global interpolation would.
        const auto n = points.size();
        for (std::size_t i = 0; i + 1 < n; ++i)
        {
            const Position& p0 = points[clamp_neighbor_index(static_cast<long>(i) - 1, n)];
            const Position& p1 = points[i];
            const Position& p2 = points[i + 1];
            const Position& p3 = points[clamp_neighbor_index(static_cast<long>(i) + 2, n)];

            const Position control1{.x = p1.x + ((p2.x - p0.x) / 6.0F), .y = p1.y + ((p2.y - p0.y) / 6.0F)};
            const Position control2{.x = p2.x - ((p3.x - p1.x) / 6.0F), .y = p2.y - ((p3.y - p1.y) / 6.0F)};

            float pc1x;
            float pc1y;
            float pc2x;
            float pc2y;
            float p2x;
            float p2y;
            provider->compute_coordinate_system(control1.x, control1.y, pc1x, pc1y);
            provider->compute_coordinate_system(control2.x, control2.y, pc2x, pc2y);
            provider->compute_coordinate_system(p2.x, p2.y, p2x, p2y);
            HPDF_Page_CurveTo(page, pc1x, pc1y, pc2x, pc2y, p2x, p2y);
        }
        HPDF_Page_Stroke(page);
    }
} // namespace docraft::backend::pdf


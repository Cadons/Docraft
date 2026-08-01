//
// Created by Matteo on 01/08/2026.
//

#include "docraft/loom/charts/docraft_chart_axis_helpers.h"

#include <algorithm>
#include <cmath>

namespace docraft::loom::charts {
    namespace {
        // Larsen "nice numbers": snaps `range` to 1, 2, 5 or 10 times a power of ten --
        // `round` picks the nearest such value (used for the step size itself), while
        // !round picks the smallest one >= range (used for the raw span, so the
        // subsequent step-size computation isn't thrown off by rounding it down first).
        float nice_number(float range, bool round)
        {
            if (range <= 0.0F)
            {
                return 1.0F;
            }
            const float exponent = std::floor(std::log10(range));
            const float fraction = range / std::pow(10.0F, exponent);
            float nice_fraction;
            if (round)
            {
                if (fraction < 1.5F) nice_fraction = 1.0F;
                else if (fraction < 3.0F) nice_fraction = 2.0F;
                else if (fraction < 7.0F) nice_fraction = 5.0F;
                else nice_fraction = 10.0F;
            }
            else
            {
                if (fraction <= 1.0F) nice_fraction = 1.0F;
                else if (fraction <= 2.0F) nice_fraction = 2.0F;
                else if (fraction <= 5.0F) nice_fraction = 5.0F;
                else nice_fraction = 10.0F;
            }
            return nice_fraction * std::pow(10.0F, exponent);
        }
    }

    std::vector<float> compute_nice_ticks(float min, float max, int target_count)
    {
        if (min > max)
        {
            std::swap(min, max);
        }
        if (min == max)
        {
            // A single-value range has no natural step; widen it so a tick step can
            // still be computed, mirroring compute_data_bounds()'s own degenerate-range
            // handling.
            min -= 0.5F;
            max += 0.5F;
        }

        const float range = nice_number(max - min, false);
        const int divisions = std::max(1, target_count - 1);
        const float step = nice_number(range / static_cast<float>(divisions), true);
        const float nice_min = std::floor(min / step) * step;
        const float nice_max = std::ceil(max / step) * step;

        std::vector<float> ticks;
        for (float v = nice_min; v <= nice_max + (step * 0.5F); v += step)
        {
            ticks.push_back(v);
        }
        return ticks;
    }

    DataBounds compute_data_bounds(const std::vector<DocraftChartSeries>& series)
    {
        bool any_point = false;
        DataBounds bounds{.min_x = 0.0F, .max_x = 1.0F, .min_y = 0.0F, .max_y = 1.0F};

        for (const auto& s : series)
        {
            for (const auto& p : s.points)
            {
                if (!any_point)
                {
                    bounds = {.min_x = p.x, .max_x = p.x, .min_y = p.y, .max_y = p.y};
                    any_point = true;
                    continue;
                }
                bounds.min_x = std::min(bounds.min_x, p.x);
                bounds.max_x = std::max(bounds.max_x, p.x);
                bounds.min_y = std::min(bounds.min_y, p.y);
                bounds.max_y = std::max(bounds.max_y, p.y);
            }
        }

        if (!any_point)
        {
            return bounds;
        }
        if (bounds.min_x == bounds.max_x)
        {
            bounds.min_x -= 0.5F;
            bounds.max_x += 0.5F;
        }
        if (bounds.min_y == bounds.max_y)
        {
            bounds.min_y -= 0.5F;
            bounds.max_y += 0.5F;
        }
        return bounds;
    }

    nodes::Position compute_axis_origin_data_space(DocraftChartAxisPosition position, const DataBounds& bounds)
    {
        switch (position)
        {
        case DocraftChartAxisPosition::kRight:
        case DocraftChartAxisPosition::kBottomRight:
            return {.x = bounds.max_x, .y = bounds.min_y};
        case DocraftChartAxisPosition::kTopLeft:
            return {.x = bounds.min_x, .y = bounds.max_y};
        case DocraftChartAxisPosition::kTopRight:
            return {.x = bounds.max_x, .y = bounds.max_y};
        case DocraftChartAxisPosition::kCenter:
            {
                float origin_x = 0.0f;
                if (bounds.min_x <= 0.0F && 0.0F <= bounds.max_x)
                {
                    origin_x = 0.0F;
                }
                else
                {
                    origin_x = (bounds.min_x + bounds.max_x) / 2.0F;
                }
                float origin_y = 0.0f;
                if (bounds.min_y <= 0.0F && 0.0F <= bounds.max_y)
                {
                    origin_y = 0.0F;
                }
                else
                {
                    origin_y = (bounds.min_y + bounds.max_y) / 2.0F;
                }
                return {.x = origin_x, .y = origin_y};
            }
        case DocraftChartAxisPosition::kLeft:
        case DocraftChartAxisPosition::kBottomLeft:
        default:
            return {.x = bounds.min_x, .y = bounds.min_y};
        }
    }
} // namespace docraft::loom::charts

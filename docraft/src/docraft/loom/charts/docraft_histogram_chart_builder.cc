//
// Created by Matteo on 02/08/2026.
//

#include "docraft/loom/charts/docraft_histogram_chart_builder.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>

#include "docraft/loom/nodes/docraft_loom_rectangle.h"

namespace docraft::loom::charts {
    namespace {
        // Fraction of a category's available slot width actually covered by bars,
        // leaving a visual gap between adjacent groups -- mirrors how most bar-chart
        // libraries reserve ~20-30% of each slot as inter-group spacing.
        constexpr float kGroupWidthFraction = 0.7F;
        // The y-axis now auto-zooms to the real data range (no adjust_data_bounds()
        // override), so the baseline a bar grows from is not necessarily true zero --
        // it may just be wherever the auto-zoomed range happens to clamp 0, or the
        // bottom/top of the plot. A bar's edge sitting flush against that baseline line
        // would misleadingly read as "this value starts at zero" even when it doesn't,
        // so every bar leaves this small pixel gap at whichever edge touches the
        // baseline instead.
        constexpr float kAxisSplitGap = 3.0F;
        // Tolerance for matching an X-tick's data-space value back to a labeled data
        // point's own x (an ordinal index) in format_x_tick_label() -- guards float
        // imprecision from map_x()/nice-tick rounding, well under the smallest possible
        // gap between two distinct integer indices.
        constexpr float kTickLabelMatchEpsilon = 0.01F;
        // Fallback category spacing used to size the half-slot padding in
        // adjust_mapped_bounds() when there's only one category (no consecutive pair of
        // ticks to measure a spacing from) -- mirrors compute_data_bounds()'s and
        // compute_nice_ticks()'s own 0.5 widening for a degenerate single-value range.
        constexpr float kDefaultCategorySpacing = 1.0F;
    }

    DataBounds DocraftHistogramChartBuilder::adjust_mapped_bounds(const DataBounds& mapped_bounds,
                                                                  const DocraftChartBuildContext& ctx) const
    {
        std::size_t max_points = 0;
        for (const auto& series : ctx.series)
        {
            max_points = std::max(max_points, series.points.size());
        }
        if (max_points == 0)
        {
            return mapped_bounds;
        }
        // Half a category's slot width, in data space -- padding the mapped domain by
        // this on each side moves every category (including the first/last) a full slot
        // away from the plot edge, matching the slot_width/group_width layout draw_series()
        // itself uses (see there for why bar_left is centered on map_x(point.x, ...)).
        const float spacing = max_points > 1
                                  ? (mapped_bounds.max_x - mapped_bounds.min_x) / static_cast<float>(max_points - 1)
                                  : kDefaultCategorySpacing;
        const float half_slot = spacing / 2.0F;
        DataBounds padded = mapped_bounds;
        padded.min_x -= half_slot;
        padded.max_x += half_slot;
        return padded;
    }

    std::string DocraftHistogramChartBuilder::format_x_tick_label(float value, const DocraftChartBuildContext& ctx) const
    {
        for (const auto& series : ctx.series)
        {
            for (std::size_t i = 0; i < series.points.size(); ++i)
            {
                if (i < series.point_labels.size() && series.point_labels[i]
                    && std::abs(series.points[i].x - value) < kTickLabelMatchEpsilon)
                {
                    return *series.point_labels[i];
                }
            }
        }
        return format_tick_label(value);
    }

    void DocraftHistogramChartBuilder::draw_series(nodes::DocraftLoomCanvas& canvas,
                                                    const DocraftChartBuildContext& ctx, const PlotRect& plot,
                                                    const DataBounds& mapped_bounds) const
    {
        if (ctx.series.empty())
        {
            return;
        }

        std::size_t max_points = 0;
        for (const auto& series : ctx.series)
        {
            max_points = std::max(max_points, series.points.size());
        }
        if (max_points == 0)
        {
            return;
        }

        // Bars grow from data-space 0 (clamped into the mapped range, mirroring
        // compute_axis_origin_data_space()'s own clamping for the same reason: a series
        // that's entirely positive or entirely negative still gets a baseline inside the
        // plot instead of one that would fall outside it).
        const float baseline_y = std::clamp(0.0F, mapped_bounds.min_y, mapped_bounds.max_y);
        const float baseline_px = map_y(baseline_y, plot, mapped_bounds);

        const float slot_width = plot.width() / static_cast<float>(max_points);
        const float group_width = slot_width * kGroupWidthFraction;
        const float bar_width = group_width / static_cast<float>(ctx.series.size());

        for (std::size_t series_index = 0; series_index < ctx.series.size(); ++series_index)
        {
            const auto& series = ctx.series[series_index];
            for (const auto& point : series.points)
            {
                const float center_x = map_x(point.x, plot, mapped_bounds);
                const float bar_left = center_x - (group_width / 2.0F) + (static_cast<float>(series_index) * bar_width);
                const float value_px = map_y(point.y, plot, mapped_bounds);
                const float raw_top = std::min(value_px, baseline_px);
                const float raw_bottom = std::max(value_px, baseline_px);
                // Pull in the baseline-side edge by kAxisSplitGap, clamped to the bar's
                // own extent so a very short bar can't invert into negative height.
                const float gap = std::min(kAxisSplitGap, raw_bottom - raw_top);
                float bar_top = raw_top;
                float bar_height = raw_bottom - raw_top;
                if (value_px < baseline_px)
                {
                    // Bar grows upward from the baseline -- baseline is the bottom edge.
                    bar_height -= gap;
                }
                else
                {
                    // Bar hangs downward from the baseline -- baseline is the top edge.
                    bar_top += gap;
                    bar_height -= gap;
                }

                auto bar = std::make_shared<nodes::DocraftLoomRectangle>();
                bar->set_width(bar_width);
                bar->set_height(bar_height);
                bar->edit_style().background_color = series.color;
                bar->set_explicit_position({.x = bar_left, .y = bar_top});
                canvas.add_child(bar);
            }
        }
    }
} // namespace docraft::loom::charts

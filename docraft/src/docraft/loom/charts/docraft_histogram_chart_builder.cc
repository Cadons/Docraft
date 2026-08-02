//
// Created by Matteo on 02/08/2026.
//

#include "docraft/loom/charts/docraft_histogram_chart_builder.h"

#include <algorithm>
#include <cstddef>

#include "docraft/loom/nodes/docraft_loom_rectangle.h"

namespace docraft::loom::charts {
    namespace {
        // Fraction of a category's available slot width actually covered by bars,
        // leaving a visual gap between adjacent groups -- mirrors how most bar-chart
        // libraries reserve ~20-30% of each slot as inter-group spacing.
        constexpr float kGroupWidthFraction = 0.7F;
    }

    DataBounds DocraftHistogramChartBuilder::adjust_data_bounds(const DataBounds& bounds) const
    {
        DataBounds widened = bounds;
        widened.min_y = std::min(widened.min_y, 0.0F);
        widened.max_y = std::max(widened.max_y, 0.0F);
        return widened;
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
                const float bar_top = std::min(value_px, baseline_px);
                const float bar_height = std::abs(value_px - baseline_px);

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

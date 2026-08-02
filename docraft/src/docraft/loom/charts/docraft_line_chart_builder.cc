//
// Created by Matteo on 02/08/2026.
//

#include "docraft/loom/charts/docraft_line_chart_builder.h"

#include <cstddef>

#include "docraft/loom/nodes/docraft_loom_circle.h"

namespace docraft::loom::charts {
    namespace {
        constexpr float kPointMarkerRadius = 2.0F;
        constexpr float kLineWidth = 1.5F;
    }

    void DocraftLineChartBuilder::draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx,
                                              const PlotRect& plot, const DataBounds& mapped_bounds) const
    {
        for (const auto& series : ctx.series)
        {
            // Step 1: map every point of this series from data space to plot pixels
            // once, up front, so both the segment-drawing loop below and the marker loop
            // reuse the same mapped coordinates instead of recomputing map_x()/map_y().
            std::vector<nodes::Position> mapped;
            mapped.reserve(series.points.size());
            for (const auto& point : series.points)
            {
                mapped.push_back({.x = map_x(point.x, plot, mapped_bounds), .y = map_y(point.y, plot, mapped_bounds)});
            }

            // Step 2: connect consecutive mapped points with straight segments, in the
            // order they appear in the series -- this is what makes it a "line" chart
            // rather than a scatter chart (see DocraftScatterChartBuilder).
            for (std::size_t i = 1; i < mapped.size(); ++i)
            {
                add_line(canvas, mapped[i - 1].x, mapped[i - 1].y, mapped[i].x, mapped[i].y, series.color, kLineWidth);
            }

            // Step 3: draw a small circle marker on top of each point, after the
            // segments, so a marker never gets painted over by a later segment.
            for (const auto& p : mapped)
            {
                auto marker = std::make_shared<nodes::DocraftLoomCircle>();
                marker->set_radius(kPointMarkerRadius);
                marker->edit_style().background_color = series.color;
                marker->set_explicit_position({.x = p.x - kPointMarkerRadius, .y = p.y - kPointMarkerRadius});
                canvas.add_child(marker);
            }
        }
    }
} // namespace docraft::loom::charts

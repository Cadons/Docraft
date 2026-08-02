//
// Created by Matteo on 02/08/2026.
//

#include "docraft/loom/charts/docraft_spline_chart_builder.h"

#include "docraft/loom/nodes/docraft_loom_circle.h"

namespace docraft::loom::charts {
    namespace {
        constexpr float kPointMarkerRadius = 2.0F;
    }

    void DocraftSplineChartBuilder::draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx,
                                                 const PlotRect& plot, const DataBounds& mapped_bounds) const
    {
        for (const auto& series : ctx.series)
        {
            // Map every point of this series from data space to plot pixels once, up
            // front, so add_curve() interpolates through the same coordinates the
            // marker loop below draws on top of.
            std::vector<nodes::Position> mapped;
            mapped.reserve(series.points.size());
            for (const auto& point : series.points)
            {
                mapped.push_back({.x = map_x(point.x, plot, mapped_bounds), .y = map_y(point.y, plot, mapped_bounds)});
            }
            // add_curve() fits a Catmull-Rom-derived Bezier through every mapped point,
            // in order, producing one smooth stroke per series -- unlike the line chart,
            // which connects points with straight segments (see
            // DocraftLineChartBuilder). A curve needs at least 2 points to draw
            // anything; a lone point still gets its marker below so a single-point
            // series isn't silently invisible.
            constexpr float kCurveWidth = 1.5F;
            add_curve(canvas, mapped, series.color, kCurveWidth);

            // Draw a small circle marker on top of each point, after the curve, so a
            // marker never gets painted over by the stroke.
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

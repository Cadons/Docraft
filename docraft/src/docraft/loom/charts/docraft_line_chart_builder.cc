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
            std::vector<nodes::Position> mapped;
            mapped.reserve(series.points.size());
            for (const auto& point : series.points)
            {
                mapped.push_back({.x = map_x(point.x, plot, mapped_bounds), .y = map_y(point.y, plot, mapped_bounds)});
            }

            for (std::size_t i = 1; i < mapped.size(); ++i)
            {
                add_line(canvas, mapped[i - 1].x, mapped[i - 1].y, mapped[i].x, mapped[i].y, series.color, kLineWidth);
            }

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

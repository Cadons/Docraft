//
// Created by Matteo on 01/08/2026.
//

#include "docraft/loom/charts/docraft_scatter_chart_builder.h"

#include "docraft/loom/nodes/docraft_loom_circle.h"

namespace docraft::loom::charts {
    namespace {
        constexpr float kPointRadius = 3.0F;
    }

    void DocraftScatterChartBuilder::draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx,
                                                  const PlotRect& plot, const DataBounds& mapped_bounds) const
    {
        for (const auto& series : ctx.series)
        {
            for (const auto& point : series.points)
            {
                auto circle = std::make_shared<nodes::DocraftLoomCircle>();
                circle->set_radius(kPointRadius);
                circle->edit_style().background_color = series.color;
                // Circle's x/y is its bounding box's top-left, not its center (center =
                // position + radius) -- offset by -radius so the point lands exactly on
                // the mapped data coordinate.
                circle->set_explicit_position({.x = map_x(point.x, plot, mapped_bounds) - kPointRadius,
                                                .y = map_y(point.y, plot, mapped_bounds) - kPointRadius});
                canvas.add_child(circle);
            }
        }
    }
} // namespace docraft::loom::charts

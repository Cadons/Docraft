//
// Created by Matteo on 02/08/2026.
//

#pragma once
#include <string_view>

#include "docraft/docraft_lib.h"
#include "docraft/loom/charts/docraft_chart_builder.h"
#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"

namespace docraft::loom::charts {
    /**
     * @brief The `<Chart style="...">` name this builder registers under (see
     * register_builtin_chart_styles()).
     */
    constexpr std::string_view kSplineStyleName = "spline";

    /**
     * @brief Spline chart style: draws one smooth curve per series through its (mapped)
     * data points -- via IDocraftLineRenderingBackend::draw_curve(), a Catmull-Rom-
     * derived Bezier interpolation -- plus a small circle marker at each point, on top
     * of the chrome (gridlines/axes/ticks/title/legend/labels) implemented by the
     * DocraftChartBuilder base class. Registered under kSplineStyleName by
     * register_builtin_chart_styles().
     */
    class DOCRAFT_LIB DocraftSplineChartBuilder : public DocraftChartBuilder
    {
    protected:
        void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx, const PlotRect& plot,
                          const DataBounds& mapped_bounds) const override;
    };
} // namespace docraft::loom::charts

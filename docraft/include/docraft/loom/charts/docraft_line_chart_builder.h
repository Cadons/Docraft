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
    constexpr std::string_view kLineStyleName = "line";

    /**
     * @brief Line chart style: draws one straight polyline per series connecting its
     * (mapped) data points in order -- via consecutive add_line() segments, since
     * DocraftLoomPolygon's non-smooth mode is a closed, fillable shape (see its class
     * doc), not the open stroked polyline a line chart needs -- plus a small circle
     * marker at each point, on top of the chrome (gridlines/axes/ticks/title/legend/
     * labels) implemented by the DocraftChartBuilder base class. Same `<Series
     * model="[[x,y],...]">` point model and marker styling as DocraftSplineChartBuilder;
     * differs only in connecting points with straight segments instead of an
     * interpolated curve. Registered under kLineStyleName by
     * register_builtin_chart_styles().
     */
    class DOCRAFT_LIB DocraftLineChartBuilder : public DocraftChartBuilder
    {
    protected:
        void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx, const PlotRect& plot,
                         const DataBounds& mapped_bounds) const override;
    };
} // namespace docraft::loom::charts

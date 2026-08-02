//
// Created by Matteo on 01/08/2026.
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
    constexpr std::string_view kScatterStyleName = "scatter";

    /**
     * @brief Scatter chart style: draws one circle per data point (per series).
     * @details Draws on top of the chrome (gridlines/axes/ticks/title/legend/labels)
     * implemented by the DocraftChartBuilder base class. Registered under
     * kScatterStyleName by register_builtin_chart_styles().
     */
    class DOCRAFT_LIB DocraftScatterChartBuilder : public DocraftChartBuilder
    {
    protected:
        void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx, const PlotRect& plot,
                          const DataBounds& mapped_bounds) const override;
    };
} // namespace docraft::loom::charts

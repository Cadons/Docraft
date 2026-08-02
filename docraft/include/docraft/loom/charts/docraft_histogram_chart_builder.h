//
// Created by Matteo on 02/08/2026.
//

#pragma once
#include <string>
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
    constexpr std::string_view kHistogramStyleName = "histogram";

    /**
     * @brief Histogram (grouped bar) chart style: draws one bar per data point, grouped
     * side-by-side by series at each shared x, on top of the chrome (gridlines/axes/
     * ticks/title/legend/labels) implemented by the DocraftChartBuilder base class. Uses
     * the same `<Series model="[[x,y],...]">` (x,y) point model as every other chart
     * style -- x positions the bar, y is its height from the baseline. The y-axis
     * auto-zooms to the real data range (no adjust_data_bounds() override, same as
     * scatter/spline), so the baseline is data-space 0 clamped into that auto-zoomed
     * range if 0 falls outside it (see the existing std::clamp() in draw_series()).
     * Because a bar flush against a non-zero baseline can misread as "starts at zero",
     * every bar leaves a small gap at whichever edge touches the baseline instead of
     * sitting directly on the axis line. Unlike the y-axis, the x-axis is categorical
     * (each x is a bar slot, not a continuous coordinate), so adjust_mapped_bounds()
     * pads it by half a slot on each side -- otherwise the first/last category's bar
     * group centers exactly on the plot edge, spilling half its width out past it. A
     * `<Series model='[{"label": value}, ...]'>` entry (the shape pie/histogram model
     * data always uses) places its bar at that entry's ordinal index and shows its key
     * as that bar's X-axis tick label, via format_x_tick_label(); plain `[x,y]` entries
     * keep the numeric tick label. Registered under kHistogramStyleName by
     * register_builtin_chart_styles().
     */
    class DOCRAFT_LIB DocraftHistogramChartBuilder : public DocraftChartBuilder
    {
    protected:
        void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx, const PlotRect& plot,
                         const DataBounds& mapped_bounds) const override;

        DataBounds adjust_mapped_bounds(const DataBounds& mapped_bounds,
                                        const DocraftChartBuildContext& ctx) const override;

        std::string format_x_tick_label(float value, const DocraftChartBuildContext& ctx) const override;
    };
} // namespace docraft::loom::charts

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
    constexpr std::string_view kHistogramStyleName = "histogram";

    /**
     * @brief Histogram (grouped bar) chart style: draws one bar per data point, grouped
     * side-by-side by series at each shared x, on top of the chrome (gridlines/axes/
     * ticks/title/legend/labels) implemented by the DocraftChartBuilder base class. Uses
     * the same `<Series model="[[x,y],...]">` (x,y) point model as every other chart
     * style -- x positions the bar, y is its height from the baseline (data-space 0,
     * clamped into range if 0 falls outside it). Registered under kHistogramStyleName by
     * register_builtin_chart_styles().
     */
    class DOCRAFT_LIB DocraftHistogramChartBuilder : public DocraftChartBuilder
    {
    protected:
        void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx, const PlotRect& plot,
                          const DataBounds& mapped_bounds) const override;

        /**
         * @brief Widens the y range to always include 0 -- a bar's height reads as "how
         * far from zero", so a range that excludes 0 (e.g. every value happens to be
         * >= 5) would make bar heights visually meaningless relative to each other.
         */
        DataBounds adjust_data_bounds(const DataBounds& bounds) const override;
    };
} // namespace docraft::loom::charts

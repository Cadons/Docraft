//
// Created by Matteo on 02/08/2026.
//

#pragma once
#include <memory>
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
    constexpr std::string_view kPieStyleName = "pie";

    /**
     * @brief Pie chart style. Unlike every other built-in style, a pie chart has no
     * Cartesian plot at all (no axes, gridlines, or ticks), so this overrides build()
     * outright instead of draw_series() -- see DocraftChartBuilder's class doc for why
     * build() is virtual. Still reuses add_text()/estimate_text_width()/
     * draw_legend_column() and the shared chrome constants (title band, legend layout)
     * from the base class, and add_polygon() to approximate each slice's curved edge as
     * a many-point fan (no dedicated arc rendering-backend primitive is needed).
     *
     * Every point across every `<Series>` becomes one slice, in order: a slice's value
     * is its point's y (x is unused -- there's no x-axis). A series contributing more
     * than one point -- the `<Series model='[{"label": value}, ...]'>` shape pie/
     * histogram model data always uses, one series holding a whole category breakdown
     * -- cycles each of its slices through default_series_color() (series.color can't
     * distinguish them); a series contributing exactly one point uses series.color
     * directly, respecting an explicit `<Series color="...">` in that common case. A
     * slice's label is its point's own `point_labels` entry if set (the `{"label":
     * value}` shape's key), else its parent series' name if set, else a positional
     * "Slice N" fallback. Points with a non-positive value are skipped. Each slice also
     * draws its share of the total as a centered whole-number percentage label (e.g.
     * "25%"), unless `<Chart show_percentage="false">` opts out via
     * DocraftChartBuildContext::show_percentage.
     * Registered under kPieStyleName by register_builtin_chart_styles().
     */
    class DOCRAFT_LIB DocraftPieChartBuilder : public DocraftChartBuilder
    {
    public:
        std::shared_ptr<nodes::DocraftLoomCanvas> build(const DocraftChartBuildContext& ctx) const override;

    protected:
        // Never called: build() is fully overridden above instead of delegating to the
        // Cartesian draw_series() hook, but the base class still declares it pure
        // virtual, so a concrete override must exist somewhere in this hierarchy.
        void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx, const PlotRect& plot,
                          const DataBounds& mapped_bounds) const override;
    };
} // namespace docraft::loom::charts

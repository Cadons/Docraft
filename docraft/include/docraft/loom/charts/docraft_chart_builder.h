//
// Created by Matteo on 01/08/2026.
//

#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "docraft/docraft_color.h"
#include "docraft/docraft_lib.h"
#include "docraft/loom/charts/docraft_chart_axis_helpers.h"
#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"

namespace docraft::loom::charts {
    /**
     * @brief The plot area's canvas-local pixel rectangle.
     * @details Computed after title/legend/axis-label chrome bands have been reserved
     * around it, so it covers only the region where gridlines, axes and data series are
     * actually drawn.
     */
    struct PlotRect
    {
        float left, top, right, bottom;
        float width() const { return right - left; }
        float height() const { return bottom - top; }
    };

    /**
     * @brief Template-method base class for chart-style builders.
     * @details build() implements every piece of chart "chrome" that is shared across
     * chart styles -- reserving title/legend/axis-label bands, computing the plot area
     * and data-to-pixel mapping, and drawing gridlines/axis lines/tick marks+labels/
     * title/legend/axis labels -- and defers only the data-series content itself (e.g.
     * circles for a scatter chart, bars for a bar chart) to draw_series(). A new chart
     * style subclasses this and implements draw_series() alone; it never needs to
     * reimplement the chrome. The drawing/measurement primitives below are `protected`,
     * not `private`, specifically so subclasses can call them -- a `private` base member
     * is invisible to derived classes and would defeat the point of sharing them.
     *
     * build() itself is virtual, not just draw_series(), because not every chart style
     * fits the Cartesian gridlines/axes/ticks chrome this base class implements -- a pie
     * chart has no axes at all. A style like that overrides build() outright instead of
     * draw_series(), reusing only the low-level primitives below (add_text(), etc.)
     * rather than the Cartesian chrome.
     */
    class DOCRAFT_LIB DocraftChartBuilder
    {
    public:
        virtual ~DocraftChartBuilder() = default;

        /**
         * @brief Builds the full chart: chrome plus this style's own data-series content.
         * @details Draws gridlines/axes/ticks/title/legend/axis labels first, then calls
         * draw_series() to let the concrete style add its own content on top.
         */
        virtual std::shared_ptr<nodes::DocraftLoomCanvas> build(const DocraftChartBuildContext& ctx) const;

    protected:
        /**
         * @brief Draws this chart style's own data-series content into `canvas`.
         * @details Called by build() after the gridlines/axes are already drawn. Convert
         * data-space coordinates to canvas-local pixels via map_x()/map_y() with the
         * given `plot`/`mapped_bounds`.
         */
        virtual void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx,
                                  const PlotRect& plot, const DataBounds& mapped_bounds) const = 0;

        /**
         * @brief Widens the raw data bounds before nice-tick computation.
         * @details The default is a no-op (auto-zoomed range, right for a scatter/spline
         * chart, where the bounds should track only the actual data). A style whose
         * visual meaning depends on a fixed reference point (e.g. a bar/histogram chart,
         * where bar height reads as "distance from zero") overrides this to fold that
         * reference value into the range, so build() never computes a baseline outside
         * the visible plot.
         */
        virtual DataBounds adjust_data_bounds(const DataBounds& bounds) const { return bounds; }

        /**
         * @brief Widens the already tick-snapped X mapping range that map_x() uses.
         * @details The default is a no-op, right for a continuous axis (scatter/spline),
         * where a point sitting exactly at the plot's left/right edge is the correct,
         * expected look. A style whose X positions are discrete category slots
         * (histogram) needs every category -- including the first/last -- to have a full
         * slot on both sides, or the outermost bar group ends up centered exactly on the
         * plot edge with half its width spilling out past it into the axis-label gutter.
         * Unlike adjust_data_bounds(), this doesn't feed nice-tick computation (so tick
         * values stay at the clean category positions, e.g. 0/1/2/3); it only widens the
         * pixel mapping those same ticks -- and draw_series()'s bars -- are placed
         * against.
         */
        virtual DataBounds adjust_mapped_bounds(const DataBounds& mapped_bounds,
                                                const DocraftChartBuildContext& ctx) const
        {
            return mapped_bounds;
        }

        /**
         * @brief Adds a line segment at the given absolute canvas-local pixel
         * coordinates.
         * @details DocraftLoomLine's own start()/end() are offsets from its anchor, which
         * for a Canvas child left at the default x/y is the canvas origin -- so the
         * coordinates pass straight through.
         */
        static void add_line(nodes::DocraftLoomCanvas& canvas, float x1, float y1, float x2, float y2,
                              const DocraftColor& color, float width);

        /**
         * @brief Adds a smooth curve through the given absolute canvas-local pixel
         * points (at least 2).
         * @details Rendered via IDocraftLineRenderingBackend::draw_curve() -- see
         * DocraftLoomSpline.
         */
        static void add_curve(nodes::DocraftLoomCanvas& canvas, const std::vector<nodes::Position>& points,
                               const DocraftColor& color, float width);

        /**
         * @brief Adds a text node at the given absolute canvas-local pixel coordinates.
         * @details `font_family`, when non-empty, is applied to the node -- callers pass
         * `ctx.font_family.value_or("")` so chart text follows the document's configured
         * default font instead of always falling back to DocraftLoomText's own Helvetica
         * default.
         */
        static void add_text(nodes::DocraftLoomCanvas& canvas, const std::string& text, float x, float y,
                              float font_size, const DocraftColor& color, bool bold = false,
                              const std::string& font_family = "");

        /**
         * @brief Adds a filled (optionally outlined) polygon at the given absolute
         * canvas-local pixel points.
         * @details Used directly for a bar/rectangle-shaped series as well as for a pie
         * chart's slice, which approximates its curved edge as a many-point fan (center +
         * a dense sampling of arc points) rather than needing a dedicated arc-drawing
         * rendering-backend primitive.
         */
        static void add_polygon(nodes::DocraftLoomCanvas& canvas, const std::vector<nodes::Position>& points,
                                 const DocraftColor& fill_color, const DocraftColor& border_color = DocraftColor(),
                                 float border_width = 0.0F);

        /**
         * @brief Creates a Canvas node sized to ctx.width x ctx.height.
         * @details The entry point every chart style's build() starts from.
         */
        static std::shared_ptr<nodes::DocraftLoomCanvas> create_canvas(const DocraftChartBuildContext& ctx);

        /**
         * @brief Approximates a text's rendered width, for centering/right-aligning
         * labels.
         * @details See the .cc for why an exact measurement isn't available here.
         */
        static float estimate_text_width(const std::string& text, float font_size);

        /**
         * @brief Formats a tick's numeric value.
         * @details Trims trailing zeros/the decimal point when the value is a whole
         * number.
         */
        static std::string format_tick_label(float value);

        /**
         * @brief Formats an X-axis tick's label for the given data-space tick value.
         * @details Default falls back to format_tick_label() (plain numeric), right for a
         * continuous axis (scatter/spline). A style whose X positions are categorical
         * indices with author-supplied labels (histogram's `{"label": value}` model
         * entries, via DocraftChartSeries::point_labels) overrides this to substitute
         * the matching category label when a tick lines up with a labeled data point,
         * keeping the numeric fallback otherwise.
         */
        virtual std::string format_x_tick_label(float value, const DocraftChartBuildContext& ctx) const;

        /**
         * @brief Maps a data-space X coordinate to a canvas-local pixel X within `plot`.
         */
        static float map_x(float value, const PlotRect& plot, const DataBounds& mapped_bounds);

        /**
         * @brief Maps a data-space Y coordinate to a canvas-local pixel Y within `plot`.
         * @details Y is inverted: canvas-local pixel Y grows downward, but chart data Y
         * should read upward on the page, matching every conventional chart's
         * orientation.
         */
        static float map_y(float value, const PlotRect& plot, const DataBounds& mapped_bounds);

        /**
         * @brief Draws a vertical column of legend rows (a color swatch + label per
         * entry) starting at (x, top_y), one row per `entries` item, in order.
         * @details Shared by the Cartesian chrome's own per-series legend (build()'s
         * step 6) and any chart style that needs a differently-keyed legend instead
         * (e.g. a pie chart's per-slice legend, which has no notion of "series" to key
         * off of).
         */
        static void draw_legend_column(nodes::DocraftLoomCanvas& canvas, float x, float top_y,
                                        const std::vector<std::pair<DocraftColor, std::string>>& entries,
                                        const DocraftColor& text_color, const std::string& font_family = "");

        // Chrome constants shared across chart styles (Cartesian or not), so a style
        // that overrides build() outright (e.g. pie) still matches the visual language
        // -- title band height, legend row layout, etc. -- of the Cartesian styles
        // sharing the same page. Constants relevant only to Cartesian chrome (axis/tick
        // bands, gridline color, ...) stay file-local to chart_builder.cc instead, since
        // no non-Cartesian style needs them.
        static constexpr float kTitleBandHeight = 24.0F;
        static constexpr float kTitleFontSize = 12.0F;
        static constexpr float kTitleTopMargin = 4.0F;
        static constexpr float kOuterEdgePadding = 10.0F;
        static constexpr float kLegendBandWidth = 110.0F;
        static constexpr float kLegendLeftGap = 10.0F;
        static constexpr float kLegendRowHeight = 14.0F;
        static constexpr float kLegendSwatchSize = 8.0F;
        static constexpr float kLegendFontSize = 8.0F;
        static constexpr float kLegendLabelGap = 4.0F;
        static constexpr float kLegendLabelVerticalNudge = 1.0F;
        static constexpr std::string_view kInkColorHex = "#333333";
        static constexpr std::string_view kDefaultSeriesLabelPrefix = "Series ";
    };
} // namespace docraft::loom::charts

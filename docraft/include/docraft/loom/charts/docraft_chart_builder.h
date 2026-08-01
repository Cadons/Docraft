//
// Created by Matteo on 01/08/2026.
//

#pragma once
#include <memory>
#include <string>

#include "docraft/docraft_color.h"
#include "docraft/docraft_lib.h"
#include "docraft/loom/charts/docraft_chart_axis_helpers.h"
#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"

namespace docraft::loom::charts {
    /**
     * @brief The plot area's canvas-local pixel rectangle, after title/legend/axis-label
     * chrome bands have been reserved around it.
     */
    struct PlotRect
    {
        float left, top, right, bottom;
        float width() const { return right - left; }
        float height() const { return bottom - top; }
    };

    /**
     * @brief Template-method base for chart-style builders.
     */
    class DOCRAFT_LIB DocraftChartBuilder
    {
    public:
        virtual ~DocraftChartBuilder() = default;

        /**
         * @brief Builds the full chart: chrome (gridlines/axes/ticks/title/legend/axis
         * labels) plus this style's own data-series content via draw_series().
         */
        std::shared_ptr<nodes::DocraftLoomCanvas> build(const DocraftChartBuildContext& ctx) const;

    protected:
        /**
         * @brief Draws this chart style's own data-series content into `canvas`, on top
         * of the gridlines/axes already drawn by build(). Convert data-space coordinates
         * to canvas-local pixels via map_x()/map_y() with the given `plot`/`mapped_bounds`.
         */
        virtual void draw_series(nodes::DocraftLoomCanvas& canvas, const DocraftChartBuildContext& ctx,
                                  const PlotRect& plot, const DataBounds& mapped_bounds) const = 0;

        /**
         * @brief Adds a line segment at the given absolute canvas-local pixel
         * coordinates. DocraftLoomLine's own start()/end() are local to its bounding box
         * (whose top-left anchor is explicit_position()) -- normalizing to the min
         * corner here keeps every caller simple regardless of which endpoint is "first".
         */
        static void add_line(nodes::DocraftLoomCanvas& canvas, float x1, float y1, float x2, float y2,
                              const DocraftColor& color, float width);

        /**
         * @brief Adds a text node at the given absolute canvas-local pixel coordinates.
         */
        static void add_text(nodes::DocraftLoomCanvas& canvas, const std::string& text, float x, float y,
                              float font_size, const DocraftColor& color, bool bold = false);

        /**
         * @brief Approximates a text's rendered width for centering/right-aligning
         * labels -- see the .cc for why an exact measurement isn't available here.
         */
        static float estimate_text_width(const std::string& text, float font_size);

        /**
         * @brief Formats a tick's numeric value, trimming trailing zeros/the decimal
         * point when the value is a whole number.
         */
        static std::string format_tick_label(float value);

        /**
         * @brief Maps a data-space X coordinate to a canvas-local pixel X within `plot`.
         */
        static float map_x(float value, const PlotRect& plot, const DataBounds& mapped_bounds);

        /**
         * @brief Maps a data-space Y coordinate to a canvas-local pixel Y within `plot`.
         * Y is inverted: canvas-local pixel Y grows downward, but chart data Y should
         * read upward on the page, matching every conventional chart's orientation.
         */
        static float map_y(float value, const PlotRect& plot, const DataBounds& mapped_bounds);
    };
} // namespace docraft::loom::charts

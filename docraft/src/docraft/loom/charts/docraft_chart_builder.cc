//
// Created by Matteo on 01/08/2026.
//

#include "docraft/loom/charts/docraft_chart_builder.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string_view>

#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/utils/docraft_utf8.h"

namespace docraft::loom::charts {
    namespace {
        constexpr float kTitleBandHeight = 24.0F;
        constexpr float kLegendBandWidth = 110.0F;
        constexpr float kBottomAxisBandHeight = 30.0F;
        constexpr float kXLabelHeight = 16.0F;
        constexpr float kLeftAxisBandWidth = 40.0F;
        constexpr float kYLabelBandHeight = 14.0F;
        constexpr float kTickLength = 4.0F;
        constexpr float kAxisLineWidth = 1.0F;
        constexpr float kGridlineWidth = 0.5F;
        constexpr float kTickLabelFontSize = 7.0F;
        constexpr float kTitleFontSize = 12.0F;
        constexpr float kLegendFontSize = 8.0F;
        constexpr float kLegendSwatchSize = 8.0F;
        constexpr float kLegendRowHeight = 14.0F;
        constexpr float kAxisTickTargetCount = 5;
        // The topmost Y tick and the rightmost X tick map to exactly plot.top/plot.right
        // by construction (see mapped_bounds below); their labels are centered on that
        // point, so without a little breathing room past the edge, the top-most tick's
        // number gets clipped by the canvas whenever there's no title/y_label band above
        // it, and likewise the right-most tick's number whenever there's no legend to its
        // right. This padding is reserved unconditionally, unlike the other bands.
        constexpr float kOuterEdgePadding = 10.0F;
        // Rough average glyph width for Helvetica-like fonts -- there is no font backend
        // available at tree-build time (only Measure/Rendering have one), so exact text
        // width can't be queried here; this heuristic is only used to *approximately*
        // center/right-align labels, not for layout-affecting sizing.
        constexpr float kGlyphWidthFactor = 0.55F;
        // Gap between a tick mark's end and the start of its numeric label, applied on
        // both axes (below the X ticks, left of the Y ticks).
        constexpr float kTickLabelGap = 2.0F;
        constexpr float kTitleTopMargin = 4.0F;
        // Horizontal gap between the plot's right edge and the legend swatches.
        constexpr float kLegendLeftGap = 10.0F;
        // Gap between a legend swatch and its label.
        constexpr float kLegendLabelGap = 4.0F;
        // Nudges a legend label up slightly so its baseline reads as centered against
        // its swatch, since DocraftLoomText positions from its own top, not its baseline.
        constexpr float kLegendLabelVerticalNudge = 1.0F;
        constexpr float kYLabelTopMargin = 1.0F;

        constexpr std::string_view kGridColorHex = "#E0E0E0";
        // Shared by both axis lines and label text -- kept as a single source of truth
        // even though the two currently happen to use the same shade.
        constexpr std::string_view kInkColorHex = "#333333";
        constexpr std::string_view kDefaultSeriesLabelPrefix = "Series ";
    } // namespace

    void DocraftChartBuilder::add_line(nodes::DocraftLoomCanvas& canvas, float x1, float y1, float x2, float y2,
                                        const DocraftColor& color, float width)
    {
        auto line = std::make_shared<nodes::DocraftLoomLine>();
        const float origin_x = std::min(x1, x2);
        const float origin_y = std::min(y1, y2);
        line->set_start({.x = x1 - origin_x, .y = y1 - origin_y});
        line->set_end({.x = x2 - origin_x, .y = y2 - origin_y});
        line->set_border_color(color);
        line->set_border_width(width);
        line->set_explicit_position({.x = origin_x, .y = origin_y});
        canvas.add_child(line);
    }

    void DocraftChartBuilder::add_text(nodes::DocraftLoomCanvas& canvas, const std::string& text, float x, float y,
                                        float font_size, const DocraftColor& color, bool bold)
    {
        auto node = std::make_shared<nodes::DocraftLoomText>(text);
        node->set_font_size(font_size);
        node->set_color(color);
        node->set_bold(bold);
        node->set_explicit_position({.x = x, .y = y});
        canvas.add_child(node);
    }

    float DocraftChartBuilder::estimate_text_width(const std::string& text, float font_size)
    {
        return static_cast<float>(docraft::utils::utf8_codepoint_count(text)) * font_size * kGlyphWidthFactor;
    }

    std::string DocraftChartBuilder::format_tick_label(float value)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << value;
        std::string s = oss.str();
        if (s.find('.') != std::string::npos)
        {
            while (s.back() == '0')
            {
                s.pop_back();
            }
            if (s.back() == '.')
            {
                s.pop_back();
            }
        }
        return s;
    }

    float DocraftChartBuilder::map_x(float value, const PlotRect& plot, const DataBounds& mapped_bounds)
    {
        return plot.left
             + (value - mapped_bounds.min_x) / (mapped_bounds.max_x - mapped_bounds.min_x) * plot.width();
    }

    float DocraftChartBuilder::map_y(float value, const PlotRect& plot, const DataBounds& mapped_bounds)
    {
        return plot.top
             + (1.0F - (value - mapped_bounds.min_y) / (mapped_bounds.max_y - mapped_bounds.min_y)) * plot.height();
    }

    std::shared_ptr<nodes::DocraftLoomCanvas> DocraftChartBuilder::build(const DocraftChartBuildContext& ctx) const
    {
        auto canvas = std::make_shared<nodes::DocraftLoomCanvas>();
        canvas->set_width(ctx.width);
        canvas->set_height(ctx.height);

        const DocraftColor grid_color{std::string(kGridColorHex)};
        const DocraftColor axis_color{std::string(kInkColorHex)};
        const DocraftColor text_color{std::string(kInkColorHex)};

        // A legend is only useful once there's something to distinguish -- 2+ series, or
        // a single series the author bothered to name.
        const bool show_legend = ctx.series.size() >= 2
                                  || (ctx.series.size() == 1 && !ctx.series.front().name.empty());

        const float title_h = ctx.title ? kTitleBandHeight : 0.0F;
        // y_label is its own horizontal band stacked below the title, above the plot --
        // NOT extra left-column width (there is no rotation to stand it up alongside the
        // Y-axis, see map_y()'s doc comment), so it never shares a row with the topmost Y
        // tick's numeric label.
        const float y_label_h = ctx.y_label ? kYLabelBandHeight : 0.0F;
        const float legend_w = show_legend ? kLegendBandWidth : 0.0F;
        const float bottom_h = kBottomAxisBandHeight + (ctx.x_label ? kXLabelHeight : 0.0F);
        const float left_w = kLeftAxisBandWidth;

        PlotRect plot{
            .left = left_w,
            .top = title_h + y_label_h + kOuterEdgePadding,
            .right = ctx.width - legend_w - kOuterEdgePadding,
            .bottom = ctx.height - bottom_h
        };
        // Guards a chart too small for its own reserved bands -- degenerate, but must not
        // divide by zero/negative below.
        if (plot.right <= plot.left)
        {
            plot.right = plot.left + 1.0F;
        }
        if (plot.bottom <= plot.top)
        {
            plot.bottom = plot.top + 1.0F;
        }

        const DataBounds data_bounds = compute_data_bounds(ctx.series);
        const auto x_ticks = compute_nice_ticks(data_bounds.min_x, data_bounds.max_x, kAxisTickTargetCount);
        const auto y_ticks = compute_nice_ticks(data_bounds.min_y, data_bounds.max_y, kAxisTickTargetCount);
        // The mapped data range spans exactly the tick range (which nice-ticks usually
        // widens slightly past the raw data), not the raw data bounds -- so points never
        // sit flush against the plot edge, and the axis-origin computation below sees the
        // same range the ticks/gridlines are drawn against.
        DataBounds mapped_bounds{
            .min_x = x_ticks.front(), .max_x = x_ticks.back(), .min_y = y_ticks.front(), .max_y = y_ticks.back()
        };
        if (mapped_bounds.min_x == mapped_bounds.max_x)
        {
            mapped_bounds.max_x += 1.0F;
        }
        if (mapped_bounds.min_y == mapped_bounds.max_y)
        {
            mapped_bounds.max_y += 1.0F;
        }

        const nodes::Position axis_origin_data = compute_axis_origin_data_space(ctx.axis_position, mapped_bounds);
        const float axis_x_px = map_x(axis_origin_data.x, plot, mapped_bounds);
        const float axis_y_px = map_y(axis_origin_data.y, plot, mapped_bounds);

        // 1. Gridlines.
        for (float xt : x_ticks)
        {
            const float px = map_x(xt, plot, mapped_bounds);
            add_line(*canvas, px, plot.top, px, plot.bottom, grid_color, kGridlineWidth);
        }
        for (float yt : y_ticks)
        {
            const float py = map_y(yt, plot, mapped_bounds);
            add_line(*canvas, plot.left, py, plot.right, py, grid_color, kGridlineWidth);
        }

        // 2. Axis lines (cross wherever axis_position places them -- may run through the
        // middle of the plot in kCenter mode).
        add_line(*canvas, plot.left, axis_y_px, plot.right, axis_y_px, axis_color, kAxisLineWidth);
        add_line(*canvas, axis_x_px, plot.top, axis_x_px, plot.bottom, axis_color, kAxisLineWidth);

        // 3. Tick marks + numeric labels. Deliberately anchored to the plot's bottom/left
        // edges (a fixed "ruler"), independent of where the axis crossing line itself
        // sits -- keeps labels readable even when axis_position places the crossing line
        // through the middle of the data (kCenter) instead of an edge.
        for (float xt : x_ticks)
        {
            const float px = map_x(xt, plot, mapped_bounds);
            add_line(*canvas, px, plot.bottom, px, plot.bottom + kTickLength, axis_color, kAxisLineWidth);
            const std::string label = format_tick_label(xt);
            const float label_w = estimate_text_width(label, kTickLabelFontSize);
            add_text(*canvas, label, px - (label_w / 2.0F), plot.bottom + kTickLength + kTickLabelGap,
                     kTickLabelFontSize, text_color);
        }
        for (float yt : y_ticks)
        {
            const float py = map_y(yt, plot, mapped_bounds);
            add_line(*canvas, plot.left - kTickLength, py, plot.left, py, axis_color, kAxisLineWidth);
            const std::string label = format_tick_label(yt);
            const float label_w = estimate_text_width(label, kTickLabelFontSize);
            add_text(*canvas, label, plot.left - kTickLength - kTickLabelGap - label_w,
                     py - (kTickLabelFontSize / 2.0F), kTickLabelFontSize, text_color);
        }

        // 4. Data series, on top of gridlines/axes -- delegated to the concrete style.
        draw_series(*canvas, ctx, plot, mapped_bounds);

        // 5. Title.
        if (ctx.title)
        {
            const float title_w = estimate_text_width(*ctx.title, kTitleFontSize);
            add_text(*canvas, *ctx.title, std::max(0.0F, (ctx.width - title_w) / 2.0F), kTitleTopMargin,
                     kTitleFontSize, text_color, /*bold=*/true);
        }

        // 6. Legend.
        if (show_legend)
        {
            const float legend_x = plot.right + kLegendLeftGap;
            for (std::size_t i = 0; i < ctx.series.size(); ++i)
            {
                const float row_y = plot.top + (static_cast<float>(i) * kLegendRowHeight);
                auto swatch = std::make_shared<nodes::DocraftLoomRectangle>();
                swatch->set_width(kLegendSwatchSize);
                swatch->set_height(kLegendSwatchSize);
                swatch->edit_style().background_color = ctx.series[i].color;
                swatch->set_explicit_position({.x = legend_x, .y = row_y});
                canvas->add_child(swatch);

                const std::string label = ctx.series[i].name.empty()
                                               ? (std::string(kDefaultSeriesLabelPrefix) + std::to_string(i + 1))
                                               : ctx.series[i].name;
                add_text(*canvas, label, legend_x + kLegendSwatchSize + kLegendLabelGap,
                         row_y - kLegendLabelVerticalNudge, kLegendFontSize, text_color);
            }
        }

        // 7. x_label/y_label. y_label is placed horizontally (above the Y axis rail)
        // rather than rotated 90 degrees -- DocraftLoomText has no rotation support today
        // (see the rendering processor's visit(DocraftLoomText*)); wiring up the
        // already-unused draw_text_matrix backend primitive is a separate feature.
        if (ctx.x_label)
        {
            const float label_w = estimate_text_width(*ctx.x_label, kTickLabelFontSize);
            add_text(*canvas, *ctx.x_label, plot.left + std::max(0.0F, (plot.width() - label_w) / 2.0F),
                     ctx.height - kXLabelHeight, kTickLabelFontSize, text_color);
        }
        if (ctx.y_label)
        {
            // Its own reserved band (plot.top already accounts for y_label_h), aligned
            // above the plot's left edge -- never shares a row with the topmost Y tick's
            // numeric label, which starts at plot.top.
            add_text(*canvas, *ctx.y_label, plot.left, title_h + kYLabelTopMargin, kTickLabelFontSize, text_color);
        }

        return canvas;
    }
} // namespace docraft::loom::charts

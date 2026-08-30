//
// Created by Matteo on 02/08/2026.
//

#include "docraft/loom/charts/docraft_pie_chart_builder.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace docraft::loom::charts {
    namespace {
        // Degrees per fan segment along a slice's arc -- smaller means a smoother-
        // looking curve at the cost of more polygon points; 3 degrees is dense enough
        // that the approximation is visually indistinguishable from a true arc at
        // typical chart sizes.
        constexpr float kDegreesPerArcSegment = 3.0F;
        constexpr float kBorderWidth = 1.0F;
        // Fraction of the radius at which a slice's percentage label is centered --
        // roughly midway out from the center, so the label sits inside the slice's body
        // rather than crowding the center point (small slices) or the outer edge (large
        // slices).
        constexpr float kPercentageLabelRadiusFraction = 0.6F;
        constexpr float kPercentageLabelFontSize = 8.0F;

        struct Slice
        {
            float value;
            DocraftColor color;
            std::string label;
        };

        // Point on the pie's circle at `angle_deg` clockwise from 12 o'clock -- the
        // conventional pie-chart start direction -- in canvas-local (Y-down) pixels.
        nodes::Position point_on_circle(float center_x, float center_y, float radius, float angle_deg)
        {
            const float angle_rad = angle_deg * (std::numbers::pi / 180.0F);
            return {.x = center_x + (radius * std::sin(angle_rad)), .y = center_y - (radius * std::cos(angle_rad))};
        }

        // Whole-number percentage of `value` within `total` (e.g. "25%") -- a pie slice's
        // share reads fine at whole-percent precision, and keeping it integer avoids
        // estimate_text_width() having to account for a variable-length decimal.
        std::string format_percentage(float value, float total)
        {
            return std::to_string(std::lround((value / total) * 100.0F)) + "%";
        }
    } // namespace

    void DocraftPieChartBuilder::draw_series(nodes::DocraftLoomCanvas&, const DocraftChartBuildContext&,
                                              const PlotRect&, const DataBounds&) const
    {
        // Intentionally unreachable: build() below is fully overridden and never calls
        // this Cartesian-chrome hook (a pie chart has no plot/mapped_bounds to give it).
    }

    std::shared_ptr<nodes::DocraftLoomCanvas> DocraftPieChartBuilder::build(const DocraftChartBuildContext& ctx) const
    {
        auto canvas = create_canvas(ctx);
        const DocraftColor text_color{std::string(kInkColorHex)};
        const std::string font_family = ctx.font_family.value_or("");

        std::vector<Slice> slices;
        for (const auto& series : ctx.series)
        {
            // A series with more than one point is the `<Series model='[{"label":
            // value}, ...]'>` shape -- one series holding a whole category breakdown,
            // rather than one series per slice -- so series.color (a single color for
            // the whole series) can't distinguish its slices; cycle default_series_color
            // per slice instead. A series with exactly one point keeps using
            // series.color, respecting an explicit <Series color="..."> in the common
            // one-point-per-series case (series index and slice index coincide there).
            const bool cycle_slice_colors = series.points.size() > 1;
            for (std::size_t i = 0; i < series.points.size(); ++i)
            {
                const auto& point = series.points[i];
                if (point.y <= 0.0F)
                {
                    continue;
                }
                const auto slice_index = slices.size();
                const std::optional<std::string> point_label =
                    i < series.point_labels.size() ? series.point_labels[i] : std::nullopt;
                std::string label;
                if (point_label)
                {
                    label = *point_label;
                }
                else if (!series.name.empty())
                {
                    label = series.name;
                }
                else
                {
                    label = "Slice " + std::to_string(slice_index + 1);
                }
                slices.push_back({.value = point.y,
                                   .color = cycle_slice_colors ? default_series_color(slice_index) : series.color,
                                   .label = std::move(label)});
            }
        }

        const float title_h = ctx.title.has_value() ? kTitleBandHeight : 0.0F;
        if (ctx.title.has_value())
        {
            const float title_w = estimate_text_width(*ctx.title, kTitleFontSize);
            add_text(*canvas, {.text = *ctx.title, .x = std::max(0.0F, (ctx.width - title_w) / 2.0F),
                                .y = kTitleTopMargin, .font_size = kTitleFontSize, .color = text_color, .bold = true,
                                .font_family = font_family});
        }

        float total = 0.0F;
        for (const auto& slice : slices)
        {
            total += slice.value;
        }
        if (slices.empty() || total <= 0.0F)
        {
            return canvas;
        }

        const float legend_w = kLegendBandWidth;
        const PlotRect pie_area{
            .left = kOuterEdgePadding,
            .top = title_h + kOuterEdgePadding,
            .right = ctx.width - legend_w - kOuterEdgePadding,
            .bottom = ctx.height - kOuterEdgePadding
        };
        const float center_x = (pie_area.left + pie_area.right) / 2.0F;
        const float center_y = (pie_area.top + pie_area.bottom) / 2.0F;
        const float radius = std::max(1.0F, std::min(pie_area.width(), pie_area.height()) / 2.0F);

        const DocraftColor separator_color = DocraftColor(ColorName::kWhite);
        const DocraftColor percentage_label_color = DocraftColor(ColorName::kWhite);
        float current_angle = 0.0F;
        std::vector<std::pair<DocraftColor, std::string>> legend_entries;
        legend_entries.reserve(slices.size());
        for (const auto& slice : slices)
        {
            const float sweep_deg = (slice.value / total) * 360.0F;
            const int segments = std::max(1, static_cast<int>(std::ceil(sweep_deg / kDegreesPerArcSegment)));

            std::vector<nodes::Position> fan;
            fan.reserve(static_cast<std::size_t>(segments) + 2);
            fan.push_back({.x = center_x, .y = center_y});
            for (int i = 0; i <= segments; ++i)
            {
                const float angle = current_angle + (sweep_deg * static_cast<float>(i) / static_cast<float>(segments));
                fan.push_back(point_on_circle(center_x, center_y, radius, angle));
            }
            add_polygon(*canvas, fan, slice.color, separator_color, kBorderWidth);

            if (ctx.show_percentage)
            {
                const float mid_angle = current_angle + (sweep_deg / 2.0F);
                const nodes::Position label_pos =
                    point_on_circle(center_x, center_y, radius * kPercentageLabelRadiusFraction, mid_angle);
                const std::string percentage = format_percentage(slice.value, total);
                const float label_w = estimate_text_width(percentage, kPercentageLabelFontSize);
                add_text(*canvas, {.text = percentage, .x = label_pos.x - (label_w / 2.0F),
                                    .y = label_pos.y - (kPercentageLabelFontSize / 2.0F),
                                    .font_size = kPercentageLabelFontSize, .color = percentage_label_color,
                                    .bold = false, .font_family = font_family});
            }

            legend_entries.emplace_back(slice.color, slice.label);
            current_angle += sweep_deg;
        }

        draw_legend_column(*canvas, {.x = pie_area.right + kLegendLeftGap, .top_y = pie_area.top,
                                      .entries = legend_entries, .text_color = text_color,
                                      .font_family = font_family});
        return canvas;
    }
} // namespace docraft::loom::charts

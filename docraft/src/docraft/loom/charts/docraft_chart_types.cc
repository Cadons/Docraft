//
// Created by Matteo on 01/08/2026.
//

#include "docraft/loom/charts/docraft_chart_types.h"

#include <array>

namespace docraft::loom::charts {
    DocraftChartBuilderRegistry& DocraftChartBuilderRegistry::instance()
    {
        static DocraftChartBuilderRegistry registry;
        return registry;
    }

    void DocraftChartBuilderRegistry::register_style(const std::string& style, DocraftChartBuilderFn builder)
    {
        builders_[style] = std::move(builder);
    }

    const DocraftChartBuilderFn* DocraftChartBuilderRegistry::find(const std::string& style) const
    {
        const auto it = builders_.find(style);
        return it != builders_.end() ? &it->second : nullptr;
    }

    DocraftColor default_series_color(std::size_t index)
    {
        // A fixed categorical palette (the well-known "tab10"-style hues), cycled by
        // index so an author who doesn't set <Series color="..."> still gets visually
        // distinct series.
        static const std::array<const char*, 6> kPalette = {
            "#1F77B4", "#FF7F0E", "#2CA02C", "#D62728", "#9467BD", "#17BECF"
        };
        return DocraftColor(std::string(kPalette[index % kPalette.size()]));
    }
} // namespace docraft::loom::charts

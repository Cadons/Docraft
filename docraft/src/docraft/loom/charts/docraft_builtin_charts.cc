//
// Created by Matteo on 01/08/2026.
//

#include <string>

#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/charts/docraft_histogram_chart_builder.h"
#include "docraft/loom/charts/docraft_line_chart_builder.h"
#include "docraft/loom/charts/docraft_pie_chart_builder.h"
#include "docraft/loom/charts/docraft_scatter_chart_builder.h"
#include "docraft/loom/charts/docraft_spline_chart_builder.h"

namespace docraft::loom::charts {
    void register_builtin_chart_styles()
    {
        auto& registry = DocraftChartBuilderRegistry::instance();
        registry.register_style(std::string(kScatterStyleName),
                                [](const DocraftChartBuildContext& ctx)
                                {
                                    return DocraftScatterChartBuilder{}.build(ctx);
                                });
        registry.register_style(std::string(kSplineStyleName),
                                [](const DocraftChartBuildContext& ctx)
                                {
                                    return DocraftSplineChartBuilder{}.build(ctx);
                                });
        registry.register_style(std::string(kLineStyleName),
                                [](const DocraftChartBuildContext& ctx)
                                {
                                    return DocraftLineChartBuilder{}.build(ctx);
                                });
        registry.register_style(std::string(kHistogramStyleName),
                                [](const DocraftChartBuildContext& ctx)
                                {
                                    return DocraftHistogramChartBuilder{}.build(ctx);
                                });
        registry.register_style(std::string(kPieStyleName),
                                [](const DocraftChartBuildContext& ctx)
                                {
                                    return DocraftPieChartBuilder{}.build(ctx);
                                });
    }
} // namespace docraft::loom::charts

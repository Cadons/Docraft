//
// Created by Matteo on 01/08/2026.
//

#include <string>

#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/charts/docraft_scatter_chart_builder.h"

namespace docraft::loom::charts {
    void register_builtin_chart_styles()
    {
        DocraftChartBuilderRegistry::instance().register_style(
            std::string(kScatterStyleName),
            [](const DocraftChartBuildContext& ctx) { return DocraftScatterChartBuilder{}.build(ctx); });
    }
} // namespace docraft::loom::charts

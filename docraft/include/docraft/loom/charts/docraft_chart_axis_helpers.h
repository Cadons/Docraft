//
// Created by Matteo on 01/08/2026.
//

#pragma once
#include <vector>

#include "docraft/docraft_lib.h"
#include "docraft/loom/charts/docraft_chart_types.h"

namespace docraft::loom::charts {
    /**
     * @brief The data-space extent covered by a chart's series.
     * @details The union of every point across every series, or the unit range [0,1] if
     * there are no points at all (avoids a divide-by-zero data-to-pixel mapping
     * downstream). A degenerate range on one axis (every point sharing the same x or y)
     * is widened by +/-0.5 around that value for the same reason.
     */
    struct DataBounds
    {
        float min_x;
        float max_x;
        float min_y;
        float max_y;
    };

    /**
     * @brief Chooses "round" tick values covering [min, max], returning approximately
     * target_count evenly-spaced ticks.
     * @details Uses the Larsen "nice numbers" heuristic: step size snaps to
     * 1/2/5 * 10^n so labels read as round numbers instead of raw fractions. The
     * returned range may extend slightly beyond [min, max] on either end.
     */
    DOCRAFT_LIB std::vector<float> compute_nice_ticks(float min, float max, int target_count = 5);

    /**
     * @brief Computes the union of every point across every series (see DataBounds).
     */
    DOCRAFT_LIB DataBounds compute_data_bounds(const std::vector<DocraftChartSeries>& series);

    /**
     * @brief Where, in data space, the two axis lines cross for a given axis_position.
     * @details kCenter clamps to the midpoint of the axis' data range when 0 falls
     * outside it, so the axes still land inside the plot area rather than off it.
     */
    DOCRAFT_LIB nodes::Position compute_axis_origin_data_space(DocraftChartAxisPosition position, const DataBounds& bounds);
} // namespace docraft::loom::charts

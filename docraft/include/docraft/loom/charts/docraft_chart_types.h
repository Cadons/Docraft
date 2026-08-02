//
// Created by Matteo on 01/08/2026.
//

#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "docraft/docraft_color.h"
#include "docraft/docraft_lib.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"

namespace docraft::loom::charts {
    /**
     * @brief Where the two axis lines cross within the plot area. `kLeft`/`kRight` pin
     * only the Y-axis edge (X-axis stays at the bottom, the classic chart look) and are
     * equivalent to `kBottomLeft`/`kBottomRight`; the four corner values additionally
     * flip the X-axis to the top; `kCenter` crosses at the data-space origin (clamped to
     * the data range if 0 falls outside it).
     */
    enum class DocraftChartAxisPosition
    {
        kLeft,
        kRight,
        kTopLeft,
        kTopRight,
        kBottomLeft,
        kBottomRight,
        kCenter
    };

    /**
     * @brief A single data series: a name (for the legend), a color, and its points in
     * data space (not yet mapped to canvas-local pixel coordinates -- each chart
     * builder does that mapping itself, since it depends on the chart's own plot area).
     */
    struct DocraftChartSeries
    {
        std::string name;
        DocraftColor color;
        std::vector<nodes::Position> points;
    };

    /**
     * @brief Everything a chart-style builder function needs to construct a chart's
     * content. Does not carry the chart's own background/border style -- that's applied
     * to the returned Canvas afterward by the tree builder, via the same
     * `apply_shape_style()` helper every other shape node already uses.
     */
    struct DocraftChartBuildContext
    {
        float width = 0.0F;
        float height = 0.0F;
        DocraftChartAxisPosition axis_position = DocraftChartAxisPosition::kLeft;
        std::optional<std::string> title;
        std::optional<std::string> x_label;
        std::optional<std::string> y_label;
        std::vector<DocraftChartSeries> series;
        // Only meaningful for the "pie" style -- whether each slice draws its centered
        // percentage-of-total label. Ignored by Cartesian styles.
        bool show_percentage = true;
    };

    /**
     * @brief Builds a chart style's content: a `DocraftLoomCanvas` populated with
     * ordinary, already-visitable primitive nodes (Line/Circle/Rectangle/Text). This is
     * the whole "plugin" contract -- no new DocraftLoomNode subclass or visitor overload
     * is ever required, since loom's visitor pattern is closed at compile time (see
     * DocraftChartBuilderRegistry's class doc for the full rationale).
     */
    using DocraftChartBuilderFn = std::function<std::shared_ptr<nodes::DocraftLoomCanvas>(const DocraftChartBuildContext&)>;

    /**
     * @brief Maps a `<Chart style="...">` value to the C++ function that builds it.
     *
     * This is Docraft's "chart plugin" mechanism: compile-time/link-time C++
     * registration, not runtime `.so`/`.dll` loading (no such infrastructure exists
     * anywhere in the codebase, and building one is a separate, much larger effort).
     * Built-in styles (e.g. "scatter") register explicitly via
     * `register_builtin_chart_styles()`, called once from
     * `DocraftLoomTreeBuilder`'s constructor -- not via static-init self-registration,
     * since Docraft defaults to a static library build where an unreferenced
     * translation unit's global initializers risk being dropped by the linker. A
     * custom/external chart style is just more C++ code (its own .cc file, linked into
     * the consumer's own binary) calling this same public `register_style()` before
     * parsing any document.
     */
    class DOCRAFT_LIB DocraftChartBuilderRegistry
    {
    public:
        static DocraftChartBuilderRegistry& instance();

        void register_style(const std::string& style, DocraftChartBuilderFn builder);

        /**
         * @brief Returns the registered builder for `style`, or nullptr if none is
         * registered under that name.
         */
        const DocraftChartBuilderFn* find(const std::string& style) const;

    private:
        DocraftChartBuilderRegistry() = default;

        std::unordered_map<std::string, DocraftChartBuilderFn> builders_;
    };

    /**
     * @brief Returns a color from a small fixed categorical palette, cycled by index --
     * used for a `<Series>` that doesn't specify its own `color`.
     */
    DOCRAFT_LIB DocraftColor default_series_color(std::size_t index);

    /**
     * @brief Registers every chart style Docraft ships out of the box (currently just
     * "scatter") into DocraftChartBuilderRegistry::instance(). Safe to call more than
     * once (re-registering a style just overwrites it with the same function).
     */
    DOCRAFT_LIB void register_builtin_chart_styles();
} // namespace docraft::loom::charts

#pragma once
#include <vector>

#include "docraft/loom/nodes/docraft_loom_stroked_line.h"

namespace docraft::loom::nodes {
    /**
     * @brief An open curve passing smoothly through an arbitrary number of points.
     *
     * Deliberately not a `DocraftLoomShape` and, like `DocraftLoomLine`, stroke-only:
     * an open curve has no interior, so there is no background color to compose. Where
     * `DocraftLoomLine` is a single segment between two points, this interpolates every
     * point it is given -- with exactly 2 points the two coincide, since the
     * interpolation degenerates to a straight segment.
     *
     * Points are Y-down offsets from the node's own origin (frame.position), the same
     * convention as `DocraftLoomPolygon`/`DocraftLoomTriangle`. The curve is rendered
     * through `IDocraftLineRenderingBackend::draw_curve()`, which interpolates a uniform
     * Catmull-Rom spline: it passes exactly through every point rather than near them.
     */
    class DOCRAFT_LIB DocraftLoomCurveLine : public DocraftLoomStrokedLine
    {
    public:
        DocraftLoomCurveLine() = default;
        ~DocraftLoomCurveLine() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const std::vector<Position>& points() const;
        void set_points(const std::vector<Position>& points);

    private:
        std::vector<Position> points_;
    };
} // docraft

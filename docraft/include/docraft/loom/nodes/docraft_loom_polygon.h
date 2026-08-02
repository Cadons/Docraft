#pragma once
#include <vector>

#include "docraft/loom/nodes/docraft_loom_shape.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Polygon node defined by an arbitrary number of points. Points are stored as
     * Y-down offsets from the node's own origin (frame.position), consistent with
     * loom's coordinate convention.
     *
     * By default (smooth() == false) this is a closed, fillable shape needing at least
     * 3 points to render. Setting smooth(true) instead renders it as an *open* curve
     * passing smoothly through every point (at least 2), via
     * IDocraftLineRenderingBackend::draw_curve() -- stroked only, never filled or
     * closed. This is how a spline chart draws its per-series curve: composed from this
     * same node/visitor rather than a dedicated node type, exactly like every other
     * chart style composes from existing primitives.
     */
    class DOCRAFT_LIB DocraftLoomPolygon : public DocraftLoomShape
    {
    public:
        DocraftLoomPolygon() = default;
        ~DocraftLoomPolygon() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const std::vector<Position>& points() const;
        void set_points(const std::vector<Position>& points);
        bool smooth() const;
        void set_smooth(bool smooth);

    private:
        std::vector<Position> points_;
        bool smooth_ = false;
    };
} // docraft
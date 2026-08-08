#pragma once
#include <vector>

#include "docraft/loom/nodes/docraft_loom_shape.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Polygon node defined by an arbitrary number of points: a closed, fillable
     * shape needing at least 3 points to render. Points are stored as Y-down offsets
     * from the node's own origin (frame.position), consistent with loom's coordinate
     * convention.
     *
     * For an *open* curve through a set of points, see `DocraftLoomCurveLine` -- that is
     * a separate node rather than a mode of this one, so a polygon is only ever a
     * polygon.
     */
    class DOCRAFT_LIB DocraftLoomPolygon : public DocraftLoomShape
    {
    public:
        DocraftLoomPolygon() = default;
        ~DocraftLoomPolygon() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const std::vector<Position>& points() const;
        void set_points(const std::vector<Position>& points);

    private:
        std::vector<Position> points_;
    };
} // docraft
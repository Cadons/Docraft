#include "docraft/loom/nodes/docraft_loom_polygon.h"

namespace docraft::loom::nodes {
    void DocraftLoomPolygon::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const std::vector<model::DocraftPoint>& DocraftLoomPolygon::points() const
    {
        return points_;
    }

    void DocraftLoomPolygon::set_points(const std::vector<model::DocraftPoint>& points)
    {
        points_ = points;
    }
} // docraft
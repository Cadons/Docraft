#include "docraft/loom/nodes/docraft_loom_polygon.h"

namespace docraft::loom::nodes {
    void DocraftLoomPolygon::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const std::vector<nodes::Position>& DocraftLoomPolygon::points() const
    {
        return points_;
    }

    void DocraftLoomPolygon::set_points(const std::vector<nodes::Position>& points)
    {
        points_ = points;
    }

    bool DocraftLoomPolygon::smooth() const
    {
        return smooth_;
    }

    void DocraftLoomPolygon::set_smooth(bool smooth)
    {
        smooth_ = smooth;
    }
} // docraft
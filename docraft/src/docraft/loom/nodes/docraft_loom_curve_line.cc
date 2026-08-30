#include "docraft/loom/nodes/docraft_loom_curve_line.h"

namespace docraft::loom::nodes {
    void DocraftLoomCurveLine::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const std::vector<Position>& DocraftLoomCurveLine::points() const
    {
        return points_;
    }

    void DocraftLoomCurveLine::set_points(const std::vector<Position>& points)
    {
        points_ = points;
    }
} // docraft

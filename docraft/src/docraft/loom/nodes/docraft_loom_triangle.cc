#include "docraft/loom/nodes/docraft_loom_triangle.h"

#include "docraft/exception/docraft_input_exceptions.h"

namespace docraft::loom::nodes {
    void DocraftLoomTriangle::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const std::vector<model::DocraftPoint>& DocraftLoomTriangle::points() const
    {
        return points_;
    }

    void DocraftLoomTriangle::set_points(const std::vector<model::DocraftPoint>& points)
    {
        if (points.size() != 3)
        {
            throw exception::InvalidInputException("Triangle requires exactly 3 points");
        }
        points_ = points;
    }
} // docraft
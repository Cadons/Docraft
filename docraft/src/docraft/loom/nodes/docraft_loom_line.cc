#include "docraft/loom/nodes/docraft_loom_line.h"

namespace docraft::loom::nodes {
    void DocraftLoomLine::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const nodes::Position& DocraftLoomLine::start() const
    {
        return start_;
    }

    void DocraftLoomLine::set_start(const nodes::Position& start)
    {
        start_ = start;
    }

    const nodes::Position& DocraftLoomLine::end() const
    {
        return end_;
    }

    void DocraftLoomLine::set_end(const nodes::Position& end)
    {
        end_ = end;
    }
} // docraft

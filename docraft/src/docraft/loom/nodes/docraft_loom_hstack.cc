#include "docraft/loom/nodes/docraft_loom_hstack.h"

namespace docraft::loom::nodes {
    void DocraftLoomHStack::accept(interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    float DocraftLoomHStack::spacing() const
    {
        return spacing_;
    }

    void DocraftLoomHStack::set_spacing(float spacing)
    {
        spacing_ = spacing;
    }

    void DocraftLoomHStack::set_weights(std::vector<float> weights)
    {
        weights_ = std::move(weights);
    }

    const std::vector<float>& DocraftLoomHStack::weights() const
    {
        return weights_;
    }
} // namespace docraft::loom::nodes

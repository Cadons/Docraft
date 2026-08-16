#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::nodes {
    DocraftLoomVStack::DocraftLoomVStack()
        : DocraftLoomLayoutContainer(12.0F)
    {
    }

    void DocraftLoomVStack::accept(interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    float DocraftLoomVStack::height() const
    {
        return height_;
    }

    void DocraftLoomVStack::set_height(float height)
    {
        height_ = height;
    }

    void DocraftLoomVStack::set_weights(std::vector<float> weights)
    {
        weights_ = std::move(weights);
    }

    const std::vector<float>& DocraftLoomVStack::weights() const
    {
        return weights_;
    }
} // namespace docraft::loom::nodes
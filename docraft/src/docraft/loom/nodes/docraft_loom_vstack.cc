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

    std::vector<float> DocraftLoomVStack::resolve_vertical_child_gaps(const DocraftLoomNode& node,
                                                                       float container_spacing)
    {
        const int n = node.children_count();
        std::vector<float> gaps;
        if (n <= 1)
        {
            return gaps;
        }
        gaps.resize(static_cast<std::size_t>(n - 1));
        for (int i = 0; i < n - 1; ++i)
        {
            gaps[static_cast<std::size_t>(i)] = resolve_child_gap(
                container_spacing, node.child(i)->margin().bottom, node.child(i + 1)->margin().top);
        }
        return gaps;
    }
} // namespace docraft::loom::nodes
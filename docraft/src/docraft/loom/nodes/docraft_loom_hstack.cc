#include "docraft/loom/nodes/docraft_loom_hstack.h"

namespace docraft::loom::nodes {
    DocraftLoomHStack::DocraftLoomHStack()
        : DocraftLoomLayoutContainer(0.0F)
    {
    }

    void DocraftLoomHStack::accept(interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    void DocraftLoomHStack::set_weights(std::vector<float> weights)
    {
        weights_ = std::move(weights);
    }

    const std::vector<float>& DocraftLoomHStack::weights() const
    {
        return weights_;
    }

    std::vector<float> DocraftLoomHStack::resolve_horizontal_child_gaps(const DocraftLoomNode& node,
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
                container_spacing, node.child(i)->margin().right, node.child(i + 1)->margin().left);
        }
        return gaps;
    }

    float DocraftLoomHStack::resolve_outer_margin(const DocraftLoomNode& node, bool leading)
    {
        const int n = node.children_count();
        if (n == 0)
        {
            return 0.0F;
        }
        return leading ? node.child(0)->margin().left : node.child(n - 1)->margin().right;
    }
} // namespace docraft::loom::nodes
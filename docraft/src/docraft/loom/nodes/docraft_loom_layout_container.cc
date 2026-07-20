#include "docraft/loom/nodes/docraft_loom_layout_container.h"

#include <algorithm>

namespace docraft::loom::nodes {
    DocraftLoomLayoutContainer::DocraftLoomLayoutContainer(float default_spacing)
        : spacing_(default_spacing)
    {
    }

    float DocraftLoomLayoutContainer::spacing() const
    {
        return spacing_;
    }

    void DocraftLoomLayoutContainer::set_spacing(float spacing)
    {
        spacing_ = spacing;
    }

    float DocraftLoomLayoutContainer::resolve_child_gap(float container_spacing, float margin_a, float margin_b)
    {
        const float requested_margin = std::max(margin_a, margin_b);
        return requested_margin > 0.0F ? requested_margin : container_spacing;
    }

    float DocraftLoomLayoutContainer::resolve_outer_margin(const DocraftLoomNode& node, bool leading)
    {
        const int n = node.children_count();
        if (n == 0)
        {
            return 0.0F;
        }
        return leading ? node.child(0)->margin().top : node.child(n - 1)->margin().bottom;
    }
} // namespace docraft::loom::nodes
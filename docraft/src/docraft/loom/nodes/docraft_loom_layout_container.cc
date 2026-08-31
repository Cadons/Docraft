#include "docraft/loom/nodes/docraft_loom_layout_container.h"

#include <algorithm>
#include <cmath>

namespace docraft::loom::nodes {
    DocraftLoomLayoutContainer::DocraftLoomLayoutContainer(float default_spacing)
        : spacing_(default_spacing)
    {
        set_padding(kDefaultPadding);
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
        // Neither neighbor asked for a margin on this touching edge -- the container's
        // own spacing (which may itself be negative) is what fills the gap.
        if (margin_a == 0.0F && margin_b == 0.0F)
        {
            return container_spacing;
        }
        // CSS-style margin collapsing: the combined gap is the larger of the two
        // positive requests plus the smaller (most negative) of the two negative
        // requests -- so a lone negative margin still pulls its neighbors together
        // instead of being discarded the way a plain max() would drop it.
        const float positive = std::max({margin_a, margin_b, 0.0F});
        const float negative = std::min({margin_a, margin_b, 0.0F});
        return positive + negative;
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

    float DocraftLoomLayoutContainer::effective_padding() const
    {
        const bool is_untouched_default = std::abs(padding() - kDefaultPadding) < 0.001F;
        if (!is_untouched_default)
        {
            return padding();
        }
        // Mirrors DocraftLoomRenderingProcessor's own has_fill/has_stroke check, so
        // "paints nothing" here means the same thing it means at paint time.
        const auto& s = style();
        const bool has_fill = s.background_color.toRGB().a > 0.0F;
        const bool has_stroke = s.border_width > 0.0F && s.border_color.toRGB().a > 0.0F;
        return (has_fill || has_stroke) ? padding() : 0.0F;
    }
} // namespace docraft::loom::nodes
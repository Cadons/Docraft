#pragma once

#include "docraft/loom/nodes/docraft_loom_shape.h"

namespace docraft::loom::nodes {
    /**
     * @brief Abstract base for loom nodes that stack children with a gap between each
     * pair of them and can optionally paint a background/border behind that stack.
     *
     * Factors out what `DocraftLoomRectangle`, `DocraftLoomVStack`, and
     * `DocraftLoomHStack` all need identically -- `spacing()`/`set_spacing()`, plus
     * `style()`/`edit_style()` inherited from `DocraftLoomShape` -- so those three
     * don't each duplicate the same getter/setter pairs. Never instantiated directly;
     * each subclass picks its own default spacing (Rectangle and VStack want visible
     * breathing room by default, HStack's shrink-to-fit columns don't).
     */
    class DOCRAFT_LIB DocraftLoomLayoutContainer : public DocraftLoomShape
    {
    public:
        /**
         * @brief Default inset (points) applied to padding() by the constructor below,
         * so a Rectangle/VStack/HStack that paints a border/background doesn't have its
         * children's content sit flush against that border unless the craft document
         * explicitly overrides it via a `padding` attribute (set_padding()).
         */
        static constexpr float kDefaultPadding = 10.0F;

        ~DocraftLoomLayoutContainer() override = default;

        float spacing() const;
        void set_spacing(float spacing);

        /**
         * @brief The gap actually placed between two adjacent children, given the two
         * touching edges (e.g. child A's bottom margin and child B's top margin for a
         * vertical stack, or A's right and B's left for a horizontal one).
         */
        static float resolve_child_gap(float container_spacing, float margin_a, float margin_b);

        /**
         * @brief The gap this container must reserve outside its first (leading=true)
         * or last (leading=false) child -- that child's own edge margin, with no
         * sibling on that side to combine it with via resolve_child_gap(). 0 if node
         * has no children. Default reads the vertical edges (top/bottom), shared as-is
         * by Rectangle/VStack; DocraftLoomHStack overrides this with its own (left/right)
         * edges.
         */
        virtual float resolve_outer_margin(const DocraftLoomNode& node, bool leading);

    protected:
        explicit DocraftLoomLayoutContainer(float default_spacing);

    private:
        float spacing_;
    };
} // namespace docraft::loom::nodes

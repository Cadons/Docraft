#pragma once

#include <vector>

#include "docraft/docraft_lib.h"
#include "docraft/loom/nodes/docraft_loom_layout_container.h"

namespace docraft::loom::nodes {
    /**
     * @brief Loom node that stacks its children vertically (top-to-bottom).
     *
     * During the measure pass, width = max child width and height = sum of child heights
     * plus spacing between each pair of adjacent children (both `spacing()` and the
     * optional background/border `style()` are inherited from
     * DocraftLoomLayoutContainer -- this lets `<Header>`/`<Body>`/`<Footer>` sections,
     * built as a DocraftLoomVStack by DocraftLoomTreeBuilder::build_section, keep
     * supporting background_color/border_color/border_width without needing
     * DocraftLoomRectangle's unrelated width_/height_/padding_ fields) -- unless an
     * explicit height() is set, in which case that height wins outright (mirrors
     * DocraftLoomRectangle's own explicit width() override).
     * During the layout pass, each child is placed below the previous one, using its
     * own natural height -- unless weights() is non-empty AND height() is explicitly
     * set, in which case that fixed height is instead divided among children by weight
     * (missing/non-positive entries default to 1.0, i.e. equal/homogeneous division),
     * mirroring DocraftLoomHStack's weighted-width distribution. Unlike HStack, a plain
     * VStack has no ambient "page height" budget to divide (pagination makes vertical
     * space effectively unbounded) -- so weights() alone has nothing well-defined to
     * divide until the author also gives the VStack its own explicit height.
     */
    class DOCRAFT_LIB DocraftLoomVStack : public DocraftLoomLayoutContainer
    {
    public:
        DocraftLoomVStack();
        ~DocraftLoomVStack() override = default;

        void accept(interfaces::DocraftLoomIVisitor& visitor) override;

        float height() const;
        void set_height(float height);

        /**
         * @brief Sets per-child weights used to divide an explicit height() among
         * children. Empty (the default) keeps today's shrink-to-fit behavior, where
         * each child simply gets its own natural height.
         */
        void set_weights(std::vector<float> weights);
        const std::vector<float>& weights() const;

        /**
         * @brief DocraftLoomLayoutContainer::resolve_child_gap() applied to every
         * adjacent pair of node's children at once, using each child's bottom/top
         * margin (the vertical-stacking counterpart of
         * DocraftLoomHStack::resolve_horizontal_child_gaps()).
         */
        std::vector<float> resolve_vertical_child_gaps(const DocraftLoomNode& node, float container_spacing);

    private:
        float height_ = 0.0F;
        std::vector<float> weights_;
    };
} // namespace docraft::loom::nodes
#pragma once

#include <vector>

#include "docraft/docraft_lib.h"
#include "docraft/loom/nodes/docraft_loom_layout_container.h"

namespace docraft::loom::nodes {
    /**
     * @brief Loom node that places its children horizontally (left-to-right).
     *
     * During the measure pass, height = max child height and width = sum of child widths
     * plus spacing between each pair of adjacent children (both `spacing()` and the
     * optional background/border `style()` are inherited from
     * DocraftLoomLayoutContainer).
     * During the layout pass, each child is placed to the right of the previous one, using
     * its own natural width -- unless weights() is non-empty, in which case the available
     * content width is instead divided among children by weight (missing/non-positive
     * entries default to 1.0, i.e. equal/homogeneous division), mirroring
     * DocraftLoomTable's column_weights.
     */
    class DOCRAFT_LIB DocraftLoomHStack : public DocraftLoomLayoutContainer
    {
    public:
        DocraftLoomHStack();
        ~DocraftLoomHStack() override = default;

        void accept(interfaces::DocraftLoomIVisitor& visitor) override;

        /**
         * @brief Sets per-child weights used to divide the available content width.
         * Empty (the default) keeps today's shrink-to-fit behavior, where each child
         * simply gets its own natural width.
         */
        void set_weights(std::vector<float> weights);
        const std::vector<float>& weights() const;

        /**
         * @brief DocraftLoomLayoutContainer::resolve_child_gap() applied to every
         * adjacent pair of node's children at once, using each child's right/left
         * margin (this is the horizontal-stacking counterpart of the vertical, per-pair
         * calls Rectangle/VStack make directly)
         **/
         std::vector<float> resolve_horizontal_child_gaps(const DocraftLoomNode& node, float container_spacing);


        float resolve_outer_margin(const DocraftLoomNode& node, bool leading) override;

    private:
        std::vector<float> weights_;
    };
} // namespace docraft::loom::nodes
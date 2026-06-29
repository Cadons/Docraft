#pragma once

#include "docraft/docraft_lib.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Loom node that places its children horizontally (left-to-right).
     *
     * During the measure pass, height = max child height and width = sum of child widths
     * plus spacing between each pair of adjacent children.
     * During the layout pass, each child is placed to the right of the previous one.
     */
    class DOCRAFT_LIB DocraftLoomHStack : public DocraftLoomNode
    {
    public:
        DocraftLoomHStack() = default;
        ~DocraftLoomHStack() override = default;

        void accept(interfaces::DocraftLoomIVisitor& visitor) override;

        float spacing() const;
        void set_spacing(float spacing);

    private:
        float spacing_ = 0.0F;
    };
} // namespace docraft::loom::nodes
#pragma once

#include "docraft/docraft_lib.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Loom node that stacks its children vertically (top-to-bottom).
     *
     * During the measure pass, width = max child width and height = sum of child heights
     * plus spacing between each pair of adjacent children.
     * During the layout pass, each child is placed below the previous one.
     */
    class DOCRAFT_LIB DocraftLoomVStack : public DocraftLoomNode
    {
    public:
        DocraftLoomVStack() = default;
        ~DocraftLoomVStack() override = default;

        void accept(interfaces::DocraftLoomIVisitor& visitor) override;

        float spacing() const;
        void set_spacing(float spacing);

    private:
        float spacing_ = 12.0F;
    };
} // namespace docraft::loom::nodes
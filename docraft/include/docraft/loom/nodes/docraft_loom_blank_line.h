#pragma once
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    class DOCRAFT_LIB DocraftLoomBlankLine : public DocraftLoomNode
    {
    public:
        DocraftLoomBlankLine() = default;
        ~DocraftLoomBlankLine() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        float height() const;
        void set_height(float height);

    private:
        // Matches DocraftLoomText's own default font_size(), so a bare <Blank /> (no
        // explicit height) reads as roughly one blank line of body text instead of an
        // imperceptible 1pt gap.
        float requested_height_ = 12.0F;
    };
} // docraft
#pragma once
#include "docraft/docraft_color.h"
#include "docraft/loom/nodes/docraft_loom_line_style.h"

namespace docraft::loom::nodes {
    /**
     * @brief Shared background/border styling for shape nodes (Rectangle, Circle,
     * Triangle, Polygon). Composition over inheritance: each shape node holds one of
     * these as a plain member, following the same idiom as DocraftLoomNode::layout_box().
     */
    struct DocraftLoomShapeStyle
    {
        DocraftColor background_color = DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 0.0F);
        DocraftColor border_color = DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 0.0F);
        float border_width = 1.0F;
        DocraftLineStyle border_style = DocraftLineStyle::kSolid;
    };
} // docraft
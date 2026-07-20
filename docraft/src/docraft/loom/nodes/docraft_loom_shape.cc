#include "docraft/loom/nodes/docraft_loom_shape.h"

namespace docraft::loom::nodes {
    const DocraftLoomShapeStyle& DocraftLoomShape::style() const
    {
        return style_;
    }

    DocraftLoomShapeStyle& DocraftLoomShape::edit_style()
    {
        return style_;
    }
} // docraft

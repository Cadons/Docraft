#pragma once

namespace docraft::loom::nodes {
    /**
     * @brief Stroke pattern for line-like nodes (Line, CurveLine) and shape borders
     * (via DocraftLoomShapeStyle::border_style).
     */
    enum class DocraftLineStyle
    {
        kSolid,
        kDashed
    };
} // docraft

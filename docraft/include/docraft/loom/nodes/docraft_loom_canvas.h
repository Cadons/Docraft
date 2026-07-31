//
// Created by Matteo on 31/07/2026.
//

#pragma once
#include "docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"

namespace docraft::loom::nodes {
    /**
     * @brief A rectangular free-form graphics container.
     *
     * Unlike `DocraftLoomRectangle`, whose children are block-stacked, `DocraftLoomCanvas`
     * positions every direct child by its own x/y relative to the canvas's own top-left
     * origin (defaulting to (0,0) when omitted), and clips their painting to its own
     * bounds -- the base building block for graphics primitives. Inherits width()/
     * height() and the composed background/border style from `DocraftLoomRectangle`, but
     * its Measure/Layout/Pagination/Render behavior is entirely its own (see the loom
     * pipeline processors' `visit(DocraftLoomCanvas*)` overloads).
     */
    class DOCRAFT_LIB DocraftLoomCanvas : public DocraftLoomRectangle
    {
    public:
        DocraftLoomCanvas();
        ~DocraftLoomCanvas() override = default;

        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;
    };
} // docraft

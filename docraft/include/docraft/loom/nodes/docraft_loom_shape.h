#pragma once
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_shape_style.h"

namespace docraft::loom::nodes {
    /**
     * @brief Common base for shape nodes (Rectangle, Circle, Triangle, Polygon), holding
     * the shared background/border styling. Concrete shapes still implement accept()
     * themselves (DocraftLoomShape is never instantiated directly).
     */
    class DOCRAFT_LIB DocraftLoomShape : public DocraftLoomNode
    {
    public:
        DocraftLoomShape() = default;
        ~DocraftLoomShape() override = default;

        const DocraftLoomShapeStyle& style() const;
        DocraftLoomShapeStyle& edit_style();

    private:
        DocraftLoomShapeStyle style_;
    };
} // docraft
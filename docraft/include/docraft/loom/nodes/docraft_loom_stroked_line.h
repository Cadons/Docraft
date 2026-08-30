#pragma once
#include "docraft/docraft_color.h"
#include "docraft/loom/nodes/docraft_loom_line_style.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Common base for stroke-only line-like nodes (Line, CurveLine), holding the
     * border color/width/style they share. Composition-over-inheritance doesn't apply
     * here the way it does for DocraftLoomShapeStyle (a plain member on each shape node):
     * Line and CurveLine have no other divergent styling to keep separate, so a shared
     * base -- mirroring DocraftLoomShape's role for fillable shapes -- avoids duplicating
     * the same three members and six accessors twice. Concrete nodes still implement
     * accept() themselves; DocraftLoomStrokedLine is never instantiated directly.
     */
    class DOCRAFT_LIB DocraftLoomStrokedLine : public DocraftLoomNode
    {
    public:
        DocraftLoomStrokedLine();
        ~DocraftLoomStrokedLine() override = default;

        const DocraftColor& border_color() const;
        void set_border_color(const DocraftColor& color);
        float border_width() const;
        void set_border_width(float width);
        DocraftLineStyle border_style() const;
        void set_border_style(DocraftLineStyle style);

    private:
        DocraftColor border_color_;
        float border_width_ = 1.0F;
        DocraftLineStyle border_style_ = DocraftLineStyle::kSolid;
    };
} // docraft

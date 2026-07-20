#pragma once
#include "docraft/docraft_color.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    class DOCRAFT_LIB DocraftLoomLine : public DocraftLoomNode
    {
    public:
        DocraftLoomLine();
        ~DocraftLoomLine() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const Position& start() const;
        void set_start(const Position& start);
        const Position& end() const;
        void set_end(const Position& end);
        const DocraftColor& border_color() const;
        void set_border_color(const DocraftColor& color);
        float border_width() const;
        void set_border_width(float width);

    private:
        Position start_{.x = 0.0F, .y = 0.0F};
        Position end_{.x = 100.0F, .y = 0.0F};
        DocraftColor border_color_;
        float border_width_ = 1.0F;
    };
} // docraft
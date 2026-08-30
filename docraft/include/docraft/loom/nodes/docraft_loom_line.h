#pragma once
#include "docraft/loom/nodes/docraft_loom_stroked_line.h"

namespace docraft::loom::nodes {
    class DOCRAFT_LIB DocraftLoomLine : public DocraftLoomStrokedLine
    {
    public:
        DocraftLoomLine() = default;
        ~DocraftLoomLine() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const Position& start() const;
        void set_start(const Position& start);
        const Position& end() const;
        void set_end(const Position& end);

    private:
        Position start_{.x = 0.0F, .y = 0.0F};
        Position end_{.x = 100.0F, .y = 0.0F};
    };
} // docraft
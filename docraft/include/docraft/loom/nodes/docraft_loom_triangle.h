#pragma once
#include <vector>

#include "docraft/loom/nodes/docraft_loom_shape.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Triangle node defined by exactly 3 points. Points are stored as Y-down offsets
     * from the node's own origin (frame.position), consistent with loom's coordinate
     * convention -- unlike legacy, which stores them Y-up and flips at render time.
     */
    class DOCRAFT_LIB DocraftLoomTriangle : public DocraftLoomShape
    {
    public:
        DocraftLoomTriangle() = default;
        ~DocraftLoomTriangle() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const std::vector<Position>& points() const;
        /**
         * @brief Sets the triangle's 3 points.
         * @throws exception::InvalidInputException if points.size() != 3.
         */
        void set_points(const std::vector<Position>& points);

    private:
        std::vector<Position> points_;
    };
} // docraft
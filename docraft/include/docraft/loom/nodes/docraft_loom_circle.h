#pragma once
#include "docraft/loom/nodes/docraft_loom_shape.h"

namespace docraft::loom::nodes {
    /**
     * @brief Circle node
     */
    class DOCRAFT_LIB DocraftLoomCircle : public DocraftLoomShape
    {
    public:
        DocraftLoomCircle() = default;
        ~DocraftLoomCircle() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        float radius() const;
        void set_radius(float radius);

    private:
        float radius_ = 0.0F;
    };
} // docraft
#pragma once
#include "docraft/loom/nodes/docraft_loom_shape.h"

namespace docraft::loom::nodes {
    /**
     * @brief Circle/ellipse node, sized by its two semi-axes.
     *
     * A circle is just the `radius_x() == radius_y()` case, so both are stored: the
     * Craft `radius` attribute sets them together (see set_radius()), while `width`/
     * `height` inscribe an ellipse in that bounding box (see set_radii()) -- its four
     * extreme points touch the middle of each box side. The node's frame position is
     * the *bounding box's* top-left corner, not the center; the center is
     * `position + (radius_x, radius_y)`.
     */
    class DOCRAFT_LIB DocraftLoomCircle : public DocraftLoomShape
    {
    public:
        DocraftLoomCircle() = default;
        ~DocraftLoomCircle() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        float radius_x() const;
        float radius_y() const;
        /**
         * @brief Sets both semi-axes to the same value, i.e. an actual circle.
         */
        void set_radius(float radius);
        /**
         * @brief Sets the two semi-axes independently, i.e. an ellipse (oval).
         */
        void set_radii(float radius_x, float radius_y);
        /**
         * @brief True when the two semi-axes are equal, which lets the rendering stage
         * pick the backend's dedicated circle primitive over the ellipse one.
         */
        bool is_circle() const;

    private:
        float radius_x_ = 0.0F;
        float radius_y_ = 0.0F;
    };
} // docraft

#pragma once

#include "docraft_lib.h"
#include <string>

namespace docraft::model {
    /**
     * @brief Positioning modes for layout.
     */
    enum class DocraftPositionType {
        kBlock,
        kAbsolute
    };

    /**
     * @brief 2D point in document space.
     */
    struct DocraftPoint {
        float x = 0.0F;
        float y = 0.0F;

        /**
         * @brief Returns a string representation of the point.
         * @return String in a human-readable format.
         */
        std::string to_string() const;
    };

    /**
     * @brief Anchor points around a rectangle and its edges.
     */
    struct DocraftAnchor {
        DocraftPoint top_left;
        DocraftPoint top_center;
        DocraftPoint top_right;
        DocraftPoint bottom_left;
        DocraftPoint bottom_center;
        DocraftPoint bottom_right;
        DocraftPoint left_center;
        DocraftPoint right_center;
    };

    /**
     * @brief Position and size transform with cached anchors.
     */
    class DOCRAFT_LIB DocraftTransform {
    public:
        /**
         * @brief Creates a zero-sized transform at origin.
         */
        DocraftTransform();

        /**
         * @brief Creates a transform with position, width, and height.
         * @param point Top-left position.
         * @param width Width in points.
         * @param height Height in points.
         */
        explicit DocraftTransform(const DocraftPoint &point, const float &width, const float &height);

        /**
         * @brief Returns a string representation of the transform.
         * @return String in a human-readable format.
         */
        std::string to_string() const;

        /**
         * @brief Get the anchors object
         *
         * @return const DocraftAnchor& Anchor positions for the rectangle.
         */
        const DocraftAnchor &anchors() const;

        /**
         * @brief Get the content anchors object
         *
         * @return const DocraftAnchor& Anchor positions for the content area.
         */
        const DocraftAnchor &content_anchors() const;

        /**
         * @brief Returns the rotation in degrees.
         * @return Rotation in degrees.
         */
        float rotation() const;

        /**
         * @brief Returns the top-left position.
         * @return Position point.
         */
        const DocraftPoint &position() const;

        /**
         * @brief Returns the center position.
         * @return Center point.
         */
        const DocraftPoint &center() const;

        /**
         * @brief Returns the width.
         * @return Width in points.
         */
        float width() const;

        /**
         * @brief Returns the height.
         * @return Height in points.
         */
        float height() const;

        /**
         * @brief Returns the padding.
         * @return Padding in points.
         */
        float padding() const;

        /**
         * @brief Sets the top-left position and recomputes anchors.
         * @param point New position.
         */
        void set_position(const DocraftPoint &point);

        /**
         * @brief Sets the width and recomputes anchors.
         * @param width New width in points.
         */
        void set_width(const float &width);

        /**
         * @brief Sets the height and recomputes anchors.
         * @param height New height in points.
         */
        void set_height(const float &height);

        /**
         * @brief Sets the padding and recomputes content anchors.
         * @param padding New padding in points.
         */
        void set_padding(const float &padding);

    private:
        /**
         * @brief Recomputes anchors and center based on geometry.
         */
        void compute_transform(const DocraftPoint &point, const float &width, const float &height);

        DocraftPoint position_;
        DocraftPoint center_;
        float width_=0.0F;
        float height_=0.0F;
        float padding_ = 10.0F;
        float rotation_ = 0.0F;
        DocraftAnchor anchor_; ///This is the anchor box of the component
        DocraftAnchor content_anchor_; ///This is the anchor box of the content, padding is applied
    };
}

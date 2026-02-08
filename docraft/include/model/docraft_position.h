#pragma once
#include <string>

namespace docraft::model {
    enum class DocraftPositionType {
        kBlock,
        kAbsolute
    };

    struct DocraftPoint {
        float x = 0.0F;
        float y = 0.0F;

        std::string to_string() const;
    };

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

    class DocraftTransform {
    public:
        DocraftTransform();

        explicit DocraftTransform(const DocraftPoint &point, const float &width, const float &height);

        std::string to_string() const;

        /**
         * @brief Get the anchors object
         *
         * @return const DocraftAnchor&
         */
        const DocraftAnchor &anchors() const;

        /**
         * @brief Get the content anchors object
         *
         * @return
         */
        const DocraftAnchor &content_anchors() const;

        float rotation() const;

        const DocraftPoint &position() const;

        const DocraftPoint &center() const;

        float width() const;

        float height() const;

        float padding() const;

        void set_position(const DocraftPoint &point);

        void set_width(const float &width);

        void set_height(const float &height);

        void set_padding(const float &padding);

    private:
        void compute_transform(const DocraftPoint &point, const float &width, const float &height);

        DocraftPoint position_;
        DocraftPoint center_;
        float width_=0.0F;
        float height_=0.0F;
        float padding_ = 0.0F;
        float rotation_ = 0.0F;
        DocraftAnchor anchor_; ///This is the anchor box of the component
        DocraftAnchor content_anchor_; ///This is the anchor box of the content, padding is applied
    };
}

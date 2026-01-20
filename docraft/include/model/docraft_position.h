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
        //TODO: remove
        DocraftPoint top_left_;
        DocraftPoint top_center_;
        DocraftPoint top_right_;
        DocraftPoint bottom_left_;
        DocraftPoint bottom_center_;
        DocraftPoint bottom_right_;
        DocraftPoint left_center_;
        DocraftPoint right_center_;

        float rotation_ = 0.0F;

    public:
        DocraftTransform();

        explicit DocraftTransform(const DocraftPoint &point, const float &width, const float &height);

        std::string to_string() const;

        //TODO: remove
        const DocraftPoint &top_left() const;

        const DocraftPoint &top_center() const;

        const DocraftPoint &top_right() const;

        const DocraftPoint &bottom_left() const;

        const DocraftPoint &bottom_center() const;

        const DocraftPoint &bottom_right() const;

        const DocraftPoint &left_center() const;

        const DocraftPoint &right_center() const;

        //-----
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

        void compute_box_size();

        DocraftPoint position_;
        DocraftPoint center_;
        float width_;
        float height_;
        float padding_=0;
        DocraftAnchor anchor_; ///This is the anchor box of the component
        DocraftAnchor content_anchor_; ///This is the anchor box of the content, padding is applied
    };
}

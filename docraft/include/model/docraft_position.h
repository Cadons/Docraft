#pragma once
#include <string>

namespace docraft::model {
    enum class DocraftPositionType{
        kBlock,
        kAbsolute
    };
    struct DocraftPoint {
        float x=0.0F;
        float y=0.0F;
        std::string to_string() const;
    };
    class DocraftTransform {
        public:
        //TODO: move to private
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
        explicit DocraftTransform(const DocraftPoint& point, const float& width, const float& height);
        std::string to_string() const;
        const DocraftPoint& top_left() const;
        const DocraftPoint& top_center() const;
        const DocraftPoint& top_right() const;
        const DocraftPoint& bottom_left() const;
        const DocraftPoint& bottom_center() const;
        const DocraftPoint& bottom_right() const;
        const DocraftPoint& left_center() const;
        const DocraftPoint& right_center() const;
        float rotation() const;
        const DocraftPoint& position() const;
        const DocraftPoint& center() const;
        float width() const;
        float height() const;

        void set_position(const DocraftPoint& point);
        void set_width(const float& width);
        void set_height(const float& height);



    private:
        void compute_transform(const DocraftPoint& point, const float& width, const float& height);
        DocraftPoint position_;
        DocraftPoint center_;
        float width_;
        float height_;
    };
}

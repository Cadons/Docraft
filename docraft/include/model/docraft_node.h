#pragma once
#include <string>

#include "docraft_pdf_context.h"

namespace docraft::model {
    enum class DocraftPositionType{
        kBlock,
        kAbsolute
    };
    struct Point {
        float x;
        float y;
    };
    struct TransformBox {
        Point top_left;
        Point top_center;
        Point top_right;
        Point bottom_left;
        Point bottom_center;
        Point bottom_right;
        Point left_center;
        Point right_center;

        float rotation = 0.0F;
        float thickness = 1.0F;

        std::string to_string() const;
    };
    /**
     * @brief Base class that represent a node in the document
     */
    class DocraftNode {
    public:
        explicit DocraftNode();

        DocraftNode(const DocraftNode &node) = default;

        explicit DocraftNode(const DocraftNode *node);

        virtual ~DocraftNode() = default;

        virtual void draw(const std::shared_ptr<DocraftPDFContext> &context) = 0;

        //getter
        [[nodiscard]] int id() const;

        [[nodiscard]] const std::string &node_name() const;

        [[nodiscard]] float x() const;

        [[nodiscard]] float y() const;

        [[nodiscard]] float width() const;

        [[nodiscard]] float height() const;

        [[nodiscard]] bool auto_fill_height() const;
        [[nodiscard]] bool auto_fill_width() const;
        [[nodiscard]] float padding() const;
        [[nodiscard]] float weight() const;
        [[nodiscard]] const TransformBox &transform_box() const;
        [[nodiscard]] DocraftPositionType position() const;
        //setter
        void set_x(float x);

        void set_y(float y);

        void set_width(float width);

        void set_height(float height);

        void set_name(const std::string &name);
        void set_auto_fill_height(bool auto_fill_height);
        void set_auto_fill_width(bool auto_fill_width);

        virtual void set_x_for_children(float x);
        virtual void set_y_for_children(float y);

        void set_padding(float padding);
        void set_weight(float weight);
        void set_transform_box(const TransformBox &transform_box);
        void set_position(DocraftPositionType position);

    private:
        void compute_transform_box();
        int id_ = 0;
        static int next_id_;
        std::string node_name_;
        float x_ = 0.0F;
        float y_ = 0.0F;
        float width_ = 0.0F;
        float height_ = 0.0F;
        bool auto_fill_height_ = true;
        bool auto_fill_width_ = true;
        float padding_ = 1.0F;
        float weight_ = 1.0F;
        TransformBox transform_box_;
        DocraftPositionType position_ = DocraftPositionType::kBlock;
    };
} // Docraft

#include "model/docraft_node.h"

#include <iostream>
#include <ostream>

namespace docraft::model {
    std::string TransformBox::to_string() const {
        std::string box_str = "TransformBox:\n";
        box_str += "  Top Left: (" + std::to_string(top_left.x) + ", " + std::to_string(top_left.y) + ")\n";
        box_str += "  Top Center: (" + std::to_string(top_center.x) + ", " + std::to_string(top_center.y) + ")\n";
        box_str += "  Top Right: (" + std::to_string(top_right.x) + ", " + std::to_string(top_right.y) + ")\n";
        box_str += "  Bottom Left: (" + std::to_string(bottom_left.x) + ", " + std::to_string(bottom_left.y) + ")\n";
        box_str += "  Bottom Center: (" + std::to_string(bottom_center.x) + ", " + std::to_string(bottom_center.y) +
                ")\n";
        box_str += "  Bottom Right: (" + std::to_string(bottom_right.x) + ", " + std::to_string(bottom_right.y) + ")\n";
        box_str += "  Left Center: (" + std::to_string(left_center.x) + ", " + std::to_string(left_center.y) + ")\n";
        box_str += "  Right Center: (" + std::to_string(right_center.x) + ", " + std::to_string(right_center.y) + ")\n";
        box_str += "  Rotation: " + std::to_string(rotation) + "\n";
        box_str += "  Thickness: " + std::to_string(thickness) + "\n";
        return box_str;
    }

    int DocraftNode::next_id_ = 0;

    DocraftNode::DocraftNode() {
        id_ = next_id_++;
    }

    DocraftNode::DocraftNode(const DocraftNode *node) : node_name_(node_name_),
                                                        id_(next_id_++),
                                                        width_(width_),
                                                        height_(height_) {
        std::cout << "pointer constructor" << std::endl;
    }

    int DocraftNode::id() const {
        return id_;
    }

    const std::string &DocraftNode::node_name() const {
        return node_name_;
    }

    float DocraftNode::x() const {
        return x_;
    }

    float DocraftNode::y() const {
        return y_;
    }

    float DocraftNode::width() const {
        return width_;
    }

    float DocraftNode::height() const {
        return height_;
    }

    bool DocraftNode::auto_fill_width() const {
        return auto_fill_width_;
    }

    bool DocraftNode::auto_fill_height() const {
        return auto_fill_height_;
    }

    float DocraftNode::padding() const {
        return padding_;
    }

    float DocraftNode::weight() const {
        return weight_;
    }

    const TransformBox &DocraftNode::transform_box() const {
        return transform_box_;
    }

    DocraftPositionType DocraftNode::position() const {
        return position_;
    }

    void DocraftNode::set_x(float x) {
        if (x<0){
            throw std::invalid_argument("X position cannot be negative");
        }
        x_ = x;
        compute_transform_box();
    }

    void DocraftNode::set_y(float y) {
        if (y<0){
           throw std::invalid_argument("Y position cannot be negative");
        }
        y_ = y;
        compute_transform_box();
    }

    void DocraftNode::set_width(float width) {
        if (width<0){
            throw std::invalid_argument("Width cannot be negative");
        }
        width_ = width;
        if (auto_fill_width_) {
            compute_transform_box();
        }
    }

    void DocraftNode::set_height(float height) {
        if (height<0){
            throw std::invalid_argument("Height cannot be negative");
        }
        height_ = height;
        if (auto_fill_height_) {
            compute_transform_box();
        }
    }

    void DocraftNode::set_name(const std::string &name) {
        node_name_ = name;
    }

    void DocraftNode::set_x_for_children(float x) {
        //Nothing to do in the base
    }

    void DocraftNode::set_y_for_children(float y) {
        //Nothing to do in the base
    }

    void DocraftNode::set_auto_fill_height(bool auto_fill_height) {
        auto_fill_height_ = auto_fill_height;
    }

    void DocraftNode::set_auto_fill_width(bool auto_fill_width) {
    }

    void DocraftNode::set_padding(float padding) {
        padding_ = padding;
    }

    void DocraftNode::set_weight(float weight) {
        if (weight < 0) {
            throw std::invalid_argument("Weight must be positive");
        }
        if (weight == 0) {
            throw std::invalid_argument("Weight cannot be zero");
        }
        if (weight > 1.0F) {
            throw std::invalid_argument("Weight must be less than or equal to one");
        }
        weight_ = weight;
    }

    void DocraftNode::set_transform_box(const TransformBox &transform_box) {
        transform_box_ = transform_box;
    }

    void DocraftNode::set_position(DocraftPositionType position) {
        position_ = position;
    }

    void DocraftNode::compute_transform_box() {
        model::TransformBox box;
        if (y()==0&&height()!=0){//this avoids having negative y at the beginning
            set_y(height());
        }
        box.top_left = {.x = x(), .y = y()};
        box.top_right = {.x = x() + width(), .y = y()};
        box.bottom_left = {.x = x(), .y = y() - height()};
        box.bottom_right = {.x = x() + width(), .y = y() - height()};
        box.top_center = {.x = x() + (width() / 2), .y = y()};
        box.bottom_center = {.x = x() + (width() / 2), .y = y() - height()};
        box.left_center = {.x = x(), .y = y() - (height() / 2)};
        box.right_center = {.x = x() + width(), .y = y() - (height() / 2)};
        set_transform_box(box);
    }
} // Docraft

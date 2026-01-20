#include "model/docraft_node.h"

#include <iostream>
#include <ostream>

namespace docraft::model {

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

    DocraftPositionType DocraftNode::position_mode() const {
        return position_mode_;
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
    
} // Docraft

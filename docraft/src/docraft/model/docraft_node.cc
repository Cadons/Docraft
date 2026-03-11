#include "docraft/model/docraft_node.h"

#include <iostream>
#include <ostream>

namespace docraft::model {

    int DocraftNode::next_id_ = 0;

    DocraftNode::DocraftNode() {
        id_ = next_id_++;
    }

    DocraftNode::DocraftNode(const DocraftNode &node)
        : DocraftTransform(node),
          transform_box_(node.transform_box_),
          id_(node.id_),
          node_name_(node.node_name_),
          auto_fill_height_(node.auto_fill_height_),
          auto_fill_width_(node.auto_fill_width_),
          weight_(node.weight_),
          position_mode_(node.position_mode_),
          page_owner_(node.page_owner_),
          z_index_(node.z_index_),
          visible_(node.visible_) {
    }

    DocraftNode::DocraftNode(const DocraftNode *node) : id_(next_id_++),
                                                        node_name_(node->node_name()),
                                                        page_owner_(node->page_owner()),
                                                        z_index_(node->z_index()) {
        std::cout << "pointer constructor" << std::endl;
    }
#pragma region Getter
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

    float DocraftNode::weight() const {
        return weight_;
    }

    DocraftPositionType DocraftNode::position_mode() const {
        return position_mode_;
    }

    int DocraftNode::page_owner() const {
        return page_owner_;
    }

    int DocraftNode::z_index() const {
        return z_index_;
    }
    bool DocraftNode::visible() const {
        return visible_;
    }
    bool DocraftNode::should_render(const std::shared_ptr<DocraftDocumentContext>& context) const {
        if (page_owner_ == -1) {
            return true;
        }
        if (!context) {
            return true;
        }
        const auto &page_backend = context->page_backend();
        if (!page_backend) {
            return true;
        }
        return page_backend->current_page_number() == static_cast<std::size_t>(page_owner_);
    }
#pragma endregion
#pragma region Setter
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
        auto_fill_width_ = auto_fill_width;
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
    void DocraftNode::set_position_mode(DocraftPositionType position_mode) {
        position_mode_ = position_mode;
    }

    void DocraftNode::set_page_owner(int page_owner) {
        page_owner_ = page_owner;
    }

    void DocraftNode::set_z_index(int z_index) {
        z_index_ = z_index;
    }

    void DocraftNode::set_visible(bool visible) {
        visible_ = visible;
    }
#pragma endregion
} // Docraft

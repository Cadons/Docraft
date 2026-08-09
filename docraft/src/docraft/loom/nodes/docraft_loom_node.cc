//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/nodes/docraft_loom_node.h"

#include <algorithm>
#include <numeric>

#include "docraft/exception/docraft_input_exceptions.h"
#define CHILD_NODE_NULL_EXCEPTION_MESSAGE "Child node cannot be null"
#define CHILD_NODE_INDEX_OUT_OF_RANGE_EXCEPTION_MESSAGE "Child index out of range"

namespace docraft::loom::nodes {
    namespace {
        template <typename T>
        bool validate_vector_index(int index, const std::vector<std::shared_ptr<T>>& vec, const std::string& message)
        {
            if (index < 0 || index >= vec.size())
            {
                throw exception::InvalidInputException(message);
            }
            return true;
        }
    }

    int DocraftLoomNode::children_count() const
    {
        return children_.size();
    }

    void DocraftLoomNode::add_child(std::shared_ptr<DocraftLoomNode> child)
    {
        if (child)
        {
            children_.push_back(child);
        }
        else
        {
            throw exception::InvalidInputException(CHILD_NODE_NULL_EXCEPTION_MESSAGE);
        }
    }

    std::shared_ptr<DocraftLoomNode> DocraftLoomNode::remove_child(int index)
    {
        if (validate_vector_index(index, children_, CHILD_NODE_INDEX_OUT_OF_RANGE_EXCEPTION_MESSAGE))
        {
            auto child = children_[index];
            children_.erase(children_.begin() + index);
            return child;
        }
        return nullptr;
    }

    std::shared_ptr<const DocraftLoomNode> DocraftLoomNode::child(int index) const
    {
        if (validate_vector_index(index, children_, CHILD_NODE_INDEX_OUT_OF_RANGE_EXCEPTION_MESSAGE))
        {
            return children_[index];
        }
        return nullptr;
    }

    std::shared_ptr<DocraftLoomNode> DocraftLoomNode::edit_child(int index)
    {
        if (validate_vector_index(index, children_, CHILD_NODE_INDEX_OUT_OF_RANGE_EXCEPTION_MESSAGE))
        {
            return children_[index];
        }
        return nullptr;
    }

    const LayoutBox& DocraftLoomNode::layout_box() const
    {
        return layout_box_;
    }

    LayoutBox& DocraftLoomNode::edit_layout_box()
    {
        return layout_box_;
    }

    void DocraftLoomNode::clear_children()
    {
        children_.clear();
    }

    std::vector<int> DocraftLoomNode::child_indices_in_paint_order() const
    {
        std::vector<int> indices(children_.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::stable_sort(indices.begin(), indices.end(), [this](int a, int b) {
            return children_[static_cast<std::size_t>(a)]->z_index()
                 < children_[static_cast<std::size_t>(b)]->z_index();
        });
        return indices;
    }

    DocraftPositionType DocraftLoomNode::position_mode() const
    {
        return position_mode_;
    }

    void DocraftLoomNode::set_position_mode(DocraftPositionType position_mode)
    {
        position_mode_ = position_mode;
    }

    const Position& DocraftLoomNode::explicit_position() const
    {
        return explicit_position_;
    }

    void DocraftLoomNode::set_explicit_position(const Position& position)
    {
        explicit_position_ = position;
    }

    int DocraftLoomNode::z_index() const
    {
        return z_index_;
    }

    void DocraftLoomNode::set_z_index(int z_index)
    {
        z_index_ = z_index;
    }

    const std::string& DocraftLoomNode::name() const
    {
        return name_;
    }

    void DocraftLoomNode::set_name(const std::string& name)
    {
        name_ = name;
    }

    float DocraftLoomNode::padding() const
    {
        return padding_;
    }

    void DocraftLoomNode::set_padding(float padding)
    {
        padding_ = padding;
    }

    const DocraftLoomMargin& DocraftLoomNode::margin() const
    {
        return margin_;
    }

    void DocraftLoomNode::set_margin(float margin)
    {
        margin_ = {.top = margin, .right = margin, .bottom = margin, .left = margin};
    }

    void DocraftLoomNode::set_margin(float top, float right, float bottom, float left)
    {
        margin_ = {.top = top, .right = right, .bottom = bottom, .left = left};
    }
} // docraft

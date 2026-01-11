#include "model/docraft_children_container_node.h"

#include <algorithm>
#include <ostream>

namespace docraft::model {
    DocraftChildrenContainerNode::DocraftChildrenContainerNode(DocraftChildrenContainerNode *node)
        : DocraftNode(node) {
        for (const auto &child: node->children()) {
            add_child(child);
        }
    }

    void DocraftChildrenContainerNode::add_child(const std::shared_ptr<DocraftNode> &child) {
        if (!child) {
            throw std::invalid_argument("Child node cannot be null");
        }
        children_.emplace_back(child);
        on_child_added();
    }

    const std::vector<std::shared_ptr<DocraftNode> > &DocraftChildrenContainerNode::children() const {
        return children_;
    }

    void DocraftChildrenContainerNode::draw_children(const std::shared_ptr<DocraftPDFContext> &context) const {
        for (const auto &child: children()) {
            child->draw(context);
        }
    }

    void DocraftChildrenContainerNode::remove_child(const std::shared_ptr<DocraftNode> &child) {
        auto it = std::ranges::find(children_, child);
        if (it != children_.end()) {
            children_.erase(it);
            on_child_removed(it - children_.begin());
        }
    }

    void DocraftChildrenContainerNode::set_x_for_children(float x) {
        for (auto &child: children_) {
            child->set_x(x);
        }
    }

    void DocraftChildrenContainerNode::set_y_for_children(float y) {
        for (auto &child: children_) {
            child->set_y(y);
        }
    }

    void DocraftChildrenContainerNode::on_child_added() {
    }

    void DocraftChildrenContainerNode::on_child_removed(int index) {
    }
    void DocraftChildrenContainerNode::clear_children() {
        children_.clear();
    }
} // docraft

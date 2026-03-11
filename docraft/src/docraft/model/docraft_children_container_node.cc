#include "docraft/model/docraft_children_container_node.h"

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

    void DocraftChildrenContainerNode::insert_child(std::size_t index, const std::shared_ptr<DocraftNode> &child) {
        if (!child) {
            throw std::invalid_argument("Child node cannot be null");
        }
        if (index > children_.size()) {
            index = children_.size();
        }
        children_.insert(children_.begin() + static_cast<std::ptrdiff_t>(index), child);
        on_child_added();
    }

    const std::vector<std::shared_ptr<DocraftNode> > &DocraftChildrenContainerNode::children() const {
        return children_;
    }

    void DocraftChildrenContainerNode::draw_children(const std::shared_ptr<DocraftDocumentContext> &context) const {
        auto ordered_children = children();
        std::ranges::stable_sort(ordered_children,
                                 [](const std::shared_ptr<DocraftNode> &a,
                                    const std::shared_ptr<DocraftNode> &b) {
                                     if (!a && !b) {
                                         return false;
                                     }
                                     if (!a) {
                                         return true;
                                     }
                                     if (!b) {
                                         return false;
                                     }
                                     return a->z_index() < b->z_index();
                                 });
        for (const auto &child: ordered_children) {
            if (child && child->should_render(context)) {
                child->draw(context);
            }
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
         //   child->set_x(x);
        }
    }

    void DocraftChildrenContainerNode::set_y_for_children(float y) {
        for (auto &child: children_) {
        //    child->set_y(y);
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

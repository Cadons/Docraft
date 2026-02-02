#include "model/docraft_layout.h"

#include <iostream>
#include <numeric>
#include <ostream>
#include <utility>

namespace docraft::model {
    DocraftLayout::DocraftLayout(DocraftLayout *node) : DocraftChildrenContainerNode(node),
                                                        orientation_(node->orientation_) {
    }

    LayoutOrientation DocraftLayout::orientation() const {
        return orientation_;
    }

    void DocraftLayout::set_orientation(LayoutOrientation orientation) {
        orientation_ = orientation;
    }

    void DocraftLayout::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        std::cout << "draw layout" << std::endl;
        draw_children(context);
    }

     std::vector<float> DocraftLayout::weights() const {
       std::vector<float> copy_weights;
        for (const auto &child: children()) {
            copy_weights.push_back(child->weight());
        }
        return copy_weights;
    }
    void DocraftLayout::on_child_added() {
      update_weights();
    }

    void DocraftLayout::on_child_removed(int  /*index*/) {
        update_weights();
    }

    void DocraftLayout::update_weights() {
        int size = children().size();

    }
    void DocraftLayout::set_weight_for_child(const int index, float weight) const {
        if (index < 0 || std::cmp_greater_equal(index ,children().size())) {
            throw std::out_of_range("Child index out of range");
        }
        children()[index]->set_weight(weight);
    }
} // docraft

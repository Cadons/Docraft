#include "layout/handler/docraft_layout_handler.h"

#include <cmath>
#include <numeric>
#include <print>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_image.h"
#include "model/docraft_text.h"
#define PADDING 20.0F

namespace docraft::layout::handler {
    void DocraftLayoutHandler::compute(const std::shared_ptr<model::DocraftLayout> &node) {
        float current_x = context()->cursor().x();
        float current_y = context()->cursor().y() - node->padding();
        const float start_y = current_y;
        int child_index = 0;
        node->set_width(context()->current_rect_width());
        float node_height = 0;
        float node_width = 0;
        for (const auto &child: node->children()) {
            context()->cursor().move_to(current_x, current_y);
            switch (node->orientation()) {
                case model::LayoutOrientation::kHorizontal: {
                    float box_width = (node->width() * child->weight()) - context()->cursor().offset_x();
                    context()->set_current_rect_width(box_width);

                    docraft::layout::DocraftLayoutEngine::layout(child, context()); // compute node layout
                    child->set_x_for_children(current_x);
                    // child->set_x(current_x);
                    // child->set_y(current_y);
                    // child->set_width(box_width);
                    node_height = std::max(node_height, child->height());
                    current_x += box_width + node->padding();
                    break;
                }
                case model::LayoutOrientation::kVertical: {
                    context()->set_current_rect_width(node->width());
                    docraft::layout::DocraftLayoutEngine::layout(child, context()); // compute node layout
                    if (std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(child)) {
                        child->set_x_for_children(current_x);
                    }
                    // child->set_x(current_x);
                    // child->set_y(current_y);
                    node_height += child->height();
                    current_y -= child->height()+node->padding();
                    break;
                }
            }
            node_width = std::max(node_width, child->width());
            //child height must be in the height of the layout box
            if (child->auto_fill_height()) {
                //if child is a layout, its height is already computed
                child->set_height(node_height);
            }
            //For Debug
            // std::print("Layout Child Node ID {} at ({}, {}), size: {}x{}\n", child->id(), child->x(), child->y(),
            //            child->width(), child->height());
        }
        if (node->height() == 0) {
            switch (node->orientation()) {
                case model::LayoutOrientation::kHorizontal: {
                    node->set_height(node_height + node->padding());
                    break;
                }
                case model::LayoutOrientation::kVertical: {
                    node->set_height(start_y - current_y + node->padding());
                    break;
                }
            }
        }
        node->set_width(node_width+node->padding());
        context()->cursor().reset_x();
        context()->cursor().move_y(start_y-node->height());
    }

    bool DocraftLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> request) {
        if (auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(request)) {
            compute(layout_node);
            // set_node_transform_box(layout_node);

            return true;
        }
        return false;
    }
} // docraft

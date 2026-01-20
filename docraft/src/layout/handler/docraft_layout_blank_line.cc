#include "layout/handler/docraft_layout_blank_line.h"

namespace docraft::layout::handler {
    void DocraftLayoutBlankLine::compute(const std::shared_ptr<model::DocraftBlankLine> &node) {
        node->set_weight(1.0F); //blank line takes full width
        set_node_position(node);
        //set_node_transform_box(node);
    }

    bool DocraftLayoutBlankLine::handle(std::shared_ptr<model::DocraftNode> request) {
        if (auto blank_line_node = std::dynamic_pointer_cast<model::DocraftBlankLine>(request)) {
            compute(blank_line_node);
          //  context()->cursor().move_y(context()->cursor().y() - blank_line_node->height());
            return true;
        }
        return false;
    }
} // docraft
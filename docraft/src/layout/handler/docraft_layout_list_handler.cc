#include "layout/handler/docraft_layout_list_handler.h"

#include <algorithm>
#include <stdexcept>

#include "generic/docraft_font_applier.h"
#include "model/docraft_text.h"

namespace docraft::layout::handler {
    void DocraftLayoutListHandler::compute(const std::shared_ptr<model::DocraftList> &node,
                                           model::DocraftTransform *box,
                                           DocraftCursor &cursor) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }

        if (node->position_mode() == model::DocraftPositionType::kBlock) {
            box->set_position({.x = cursor.x(), .y = cursor.y()});
        } else {
            box->set_position({.x = node->position().x, .y = node->position().y});
        }
        // Keep width/height as computed from children; do not override to zero.
    }

    void DocraftLayoutListHandler::compute_children(
        const std::shared_ptr<model::DocraftList> &node,
        DocraftCursor &cursor,
        std::vector<model::DocraftTransform> &child_boxes,
        const std::function<model::DocraftTransform(
            const std::shared_ptr<model::DocraftNode> &, DocraftCursor &)> &layout_child,
        float max_width) const {
        if (!node) {
            throw std::invalid_argument("list node is null");
        }
        node->update_items();
        node->clear_markers();
        const float saved_available_space = context()->available_space();
        const float list_available_width = max_width;
        for (std::size_t i = 0; i < node->children().size(); ++i) {
            auto text_child = std::dynamic_pointer_cast<model::DocraftText>(node->children()[i]);
            if (!text_child) {
                throw std::invalid_argument("List items must be Text nodes");
            }

            const float item_x = cursor.x();
            const float item_y = cursor.y();

            auto marker_text = std::make_shared<model::DocraftText>(node->marker_for_index(static_cast<int>(i)));
            marker_text->set_font_name(text_child->font_name());
            marker_text->set_font_size(text_child->font_size());
            marker_text->set_style(text_child->style());
            marker_text->set_color(text_child->color());
            float marker_width = 0.0F;
            float marker_size = 0.0F;
            if (node->marker_is_box()) {
                marker_size = text_child->font_size() * 0.6F;
                marker_width = marker_size;
            } else {
                generic::DocraftFontApplier font_applier(context());
                font_applier.apply_font(marker_text);
                marker_width = context()->text_backend()->measure_text_width(marker_text->text());
            }
            const float marker_gap = marker_width > 0.0F ? 6.0F : 0.0F;

            const float content_width = std::max(0.0F, list_available_width - marker_width - marker_gap);
            context()->set_current_rect_width(content_width);
            cursor.move_to(item_x + marker_width + marker_gap, item_y);

            const float original_padding = text_child->padding();
            text_child->set_padding(original_padding * 0.5F);
            auto child_box = layout_child(text_child, cursor);
            text_child->set_padding(original_padding);

            const auto lines = text_child->lines();
            float marker_y = lines.empty() ? item_y : lines.front()->position().y;
            if (node->marker_is_box()) {
                // Align vertically centered to the first line's block.
                const float text_height = child_box.height();
                const float marker_center_y = marker_y + text_height / 2.0F;
                const float adjusted_marker_y = marker_center_y - marker_size / 2.0F;
                const float max_marker_y = marker_y + text_height - 1.2F * marker_size;
                const float final_marker_y = std::min(adjusted_marker_y, max_marker_y);
                const float min_marker_y = marker_y;
                marker_y = std::max(final_marker_y, min_marker_y);
            }
            const auto marker_kind = node->marker_is_box()
                                         ? model::DocraftList::Marker::Kind::kBox
                                         : model::DocraftList::Marker::Kind::kText;
            node->set_marker(static_cast<int>(i), marker_text->text(),
                             {.x = item_x, .y = marker_y},
                             marker_kind, marker_size);

            model::DocraftTransform item_box({.x = item_x, .y = child_box.position().y},
                                             marker_width + marker_gap + child_box.width(),
                                             child_box.height());
            child_boxes.emplace_back(item_box);

            cursor.move_to(item_x, cursor.y());
        }
        context()->set_current_rect_width(saved_available_space);
    }

    bool DocraftLayoutListHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                          model::DocraftTransform *result,
                                          DocraftCursor &cursor) {
        if (auto list_node = std::dynamic_pointer_cast<model::DocraftList>(request)) {
            compute(list_node, result, cursor);
            return true;
        }
        return false;
    }
} // docraft::layout::handler

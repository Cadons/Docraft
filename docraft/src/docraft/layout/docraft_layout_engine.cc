#include "docraft/layout/docraft_layout_engine.h"

#include <algorithm>
#include <functional>
#include <limits>

#include "docraft/layout/handler/docraft_basic_layout_handler.h"
#include "docraft/layout/handler/docraft_layout_blank_line.h"
#include "docraft/layout/handler/docraft_layout_handler.h"
#include "docraft/layout/handler/docraft_layout_list_handler.h"
#include "docraft/layout/handler/docraft_layout_table_handler.h"
#include "docraft/layout/handler/docraft_layout_text_handler.h"

#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_footer.h"
#include "docraft/model/docraft_list.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_foreach.h"
#include "docraft/model/docraft_new_page.h"

namespace {
    /**
     * Counts how many rows of the table can fit in the remaining vertical space of the body section.
     * It checks the bottom y-coordinate of each row's cells against the body bottom y-coordinate.
     * If a row has no cells, it is considered not to fit.
     * @param table The table to check.
     * @param body_bottom_y The y-coordinate of the bottom of the body section.
     * @return The number of rows that can fit in the remaining space.
     */
    std::size_t count_rows_fit_horizontal(const docraft::model::DocraftTable &table, float body_bottom_y) {
        std::size_t fit = 0;
        const auto grid = table.content_nodes();
        for (const auto &row: grid) {
            float row_bottom = std::numeric_limits<float>::infinity();
            bool found = false;
            for (const auto &cell: row) {
                if (cell) {
                    //check all cells in the row and find the lowest bottom y-coordinate
                    row_bottom = cell->anchors().bottom_left.y;
                    found = true;
                    break;
                }
            }
            //if no cells found in the row, consider it not to fit
            if (!found) {
                return fit;
            }
            //if the lowest bottom y-coordinate of the row is above the body bottom y-coordinate, it fits
            if (row_bottom < body_bottom_y) {
                return fit;
            }
            ++fit; //otherwise, it fits and we check the next row
        }
        return fit;
    }

    /**
    * Similar to count_rows_fit_horizontal but checks the title rows of the table instead.
    * If a title row has no cells, it is considered not to fit.
    */
    std::size_t count_rows_fit_vertical(const docraft::model::DocraftTable &table, float body_bottom_y) {
        std::size_t fit = 0;
        const auto &titles = table.title_nodes();
        for (const auto &title: titles) {
            if (!title) {
                return fit;
            }
            const float row_bottom = title->anchors().bottom_left.y;
            if (row_bottom < body_bottom_y) {
                return fit;
            }
            ++fit;
        }
        return fit;
    }
} // namespace

namespace docraft::layout {
    DocraftLayoutEngine::DocraftLayoutEngine(const std::shared_ptr<DocraftDocumentContext> &context,
                                             const bool reset_cursor)
        : context_(context) {
        configure_handlers(context);
        if (reset_cursor && context) {
            context->cursor().move_to(0, context->page_height());
        }
    }

    void DocraftLayoutEngine::
    configure_handlers(const std::shared_ptr<DocraftDocumentContext> &context) {
        list_handler_ = nullptr;
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTextHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTableHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutBlankLine>(context));
        auto list_handler = std::make_unique<handler::DocraftLayoutListHandler>(context);
        list_handler_ = list_handler.get();
        handlers_.emplace_back(std::move(list_handler));
        handlers_.emplace_back(std::make_unique<handler::DocraftBasicLayoutHandler>(context));
    }

    const std::shared_ptr<DocraftDocumentContext> &DocraftLayoutEngine::context() const {
        return context_;
    }

    model::DocraftTransform DocraftLayoutEngine::compute_max_rect(const std::vector<model::DocraftTransform> &boxes) {
        if (boxes.empty()) {
            return model::DocraftTransform{};
        }

        float min_x = boxes[0].anchors().top_left.x;
        float max_x = boxes[0].anchors().top_right.x;
        float min_y = boxes[0].anchors().bottom_left.y;
        float max_y = boxes[0].anchors().top_left.y;

        for (const auto &box: boxes) {
            min_x = std::min(min_x, box.anchors().top_left.x);
            max_x = std::max(max_x, box.anchors().top_right.x);
            min_y = std::min(min_y, box.anchors().bottom_left.y);
            max_y = std::max(max_y, box.anchors().top_left.y);
        }

        float width = max_x - min_x;
        float height = max_y - min_y;

        return model::DocraftTransform({.x = min_x, .y = max_y}, width, height);
    }

    bool DocraftLayoutEngine::compute_node(const std::shared_ptr<model::DocraftNode> &node,
                                           model::DocraftTransform *box,
                                           DocraftCursor &cursor) const {
        for (const auto &handler: handlers_) {
            if (handler->handle(node, box, cursor)) {
                return true;
            }
        }
        return false;
    }


    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node->visible()) {
            return model::DocraftTransform{};
        }
        auto &cursor = context()->cursor();
        return compute_layout(node, cursor);
    }

    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                                DocraftCursor &cursor) {
        if (!node->visible()) {
            return model::DocraftTransform{};
        }
        std::vector<model::DocraftTransform> child_boxes;
        float max_width = context()->available_space();
        const float flow_origin_x = cursor.x();
        const float flow_origin_y = cursor.y();
        const bool is_absolute = (node->position_mode() == model::DocraftPositionType::kAbsolute);
        DocraftCursor local_cursor = cursor;
        DocraftCursor &active_cursor = is_absolute ? local_cursor : cursor;
        if (is_absolute) {
            active_cursor.move_to(flow_origin_x + node->position().x, flow_origin_y - node->position().y);
        }
        DocraftCursor local_node_cursor = active_cursor;
        DocraftCursor *layout_cursor = &active_cursor;
        const bool layout_text_flow = !is_absolute &&
                                      (std::dynamic_pointer_cast<model::DocraftText>(node) ||
                                       std::dynamic_pointer_cast<model::DocraftList>(node));
        if (layout_text_flow) {
            local_node_cursor.move_to(active_cursor.x(), active_cursor.y());
            layout_cursor = &local_node_cursor;
        }
        bool rect_uses_origin_cursor = false;
        DocraftCursor rect_origin_cursor = active_cursor;
        if (auto rect_container = std::dynamic_pointer_cast<model::DocraftRectangle>(node)) {
            if (std::dynamic_pointer_cast<model::DocraftSection>(node)) {
                // Sections handle their own padding/margins; don't override cursor here.
            } else if (!rect_container->children().empty()) {
                DocraftCursor rect_cursor = active_cursor;
                if (rect_container->position_mode() == model::DocraftPositionType::kAbsolute) {
                    rect_cursor.move_to(flow_origin_x + rect_container->position().x,
                                        flow_origin_y - rect_container->position().y);
                } else {
                    rect_cursor.move_to(active_cursor.x(), active_cursor.y());
                }
                // Ensure children are laid out relative to the rectangle's top-left.
                rect_container->set_position({.x = rect_cursor.x(), .y = rect_cursor.y()});
                rect_origin_cursor = rect_cursor;
                rect_uses_origin_cursor = true;
                local_node_cursor = rect_cursor;
                layout_cursor = &local_node_cursor;
                if (rect_container->width() > 0.0F) {
                    max_width = rect_container->width();
                }
                context()->set_current_rect_width(max_width);
            }
        }
        std::shared_ptr<model::DocraftSection> section_node = nullptr;
        float section_content_bottom = 0.0F;
        bool section_has_bounds = false;
        if (auto section = std::dynamic_pointer_cast<model::DocraftSection>(node)) {
            section_node = section;
            const float left_margin = section_node->margin_left();
            const float right_margin = section_node->margin_right();
            const float top_margin = section_node->margin_top();
            const float padding = std::max(0.0F, section_node->padding());
            float base_x = section_node->position().x;
            if (base_x == 0.0F && left_margin > 0.0F) {
                base_x = left_margin;
            }
            active_cursor.move_to(base_x, active_cursor.y() - top_margin - padding); // vertical inset only
            if (section_node->width() > 0.0F) {
                max_width = section_node->width();
            } else {
                max_width = max_width - left_margin - right_margin; // width from left margin to right margin
            }
            context()->set_current_rect_width(max_width);
            if (section_node->height() > 0.0F) {
                section_content_bottom = section_node->position().y - section_node->height() + section_node->
                                         margin_bottom() + padding;
                section_has_bounds = true;
            }
        }
        if (std::dynamic_pointer_cast<model::DocraftLayout>(node)) {
            //Move the cursor direction based on layout orientation to layout children correctly
            auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(node);
            if (layout_node->orientation() == model::LayoutOrientation::kHorizontal) {
                layout_cursor->push_direction(DocraftCursorDirection::kHorizontal);
            } else {
                layout_cursor->push_direction(DocraftCursorDirection::kVertical);
            }
        }
        //Process nodes from here
        if (auto list_node = std::dynamic_pointer_cast<model::DocraftList>(node)) {
            if (!list_handler_) {
                throw std::runtime_error("DocraftLayoutListHandler not configured");
            }
            DocraftCursor list_cursor = *layout_cursor;
            list_handler_->compute_children(
                list_node,
                list_cursor,
                child_boxes,
                [this](const std::shared_ptr<model::DocraftNode> &child, DocraftCursor &child_cursor) {
                    return compute_layout(child, child_cursor);
                },
                max_width);
        } else if (std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            auto container_node = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node);
            if (!container_node->children().empty()) {
                bool has_variable_weight = false;
                for (const auto &child: container_node->children()) {
                    if (child->weight() == -1.0F) {
                        has_variable_weight = true;
                        break;
                    }
                }
                if (has_variable_weight) {
                    // If no weight is assigned to children, distribute weights equally

                    const float equal_weight = 1.0F / static_cast<float>(container_node->children().size());
                    for (const auto &child: container_node->children()) {
                        child->set_weight(equal_weight);
                    }
                }
            }
            const float saved_available_space = context()->available_space();
            const bool is_horizontal = (layout_cursor->direction() == DocraftCursorDirection::kHorizontal);
            const std::size_t child_count = container_node->children().size();
            float available_width_for_children = max_width;
            if (is_horizontal && child_count > 1) {
                // Reserve fixed gaps between columns, then distribute the remaining width by weight.
                const float total_spacing = kHorizontalSpacing_ * static_cast<float>(child_count - 1);
                available_width_for_children = std::max(0.0F, max_width - total_spacing);
            }
            for (const auto &child: container_node->children()) {
                if (child->z_index() == node->z_index()) {
                    if (is_horizontal) {
                        context()->set_current_rect_width(available_width_for_children * child->weight());
                    } else {
                        context()->set_current_rect_width(max_width);
                    }
                    const float start_x = layout_cursor->x();
                    const float start_y = layout_cursor->y();
                    const float allocated_width = is_horizontal
                                                      ? available_width_for_children * child->weight()
                                                      : max_width;
                    auto child_box = compute_layout(child, *layout_cursor);


                    child_boxes.emplace_back(child_box);
                    if (is_horizontal) {
                        // Advance to next column using allocated width, not the rendered width.
                        layout_cursor->move_to(start_x + allocated_width + kHorizontalSpacing_, start_y);
                    }
                    if (section_has_bounds && layout_cursor->y() < section_content_bottom) {
                        layout_cursor->set_y(section_content_bottom);
                    }
                }
            }
            context()->set_current_rect_width(saved_available_space);
        }

        auto max_rect = compute_max_rect(child_boxes);

        if (rect_uses_origin_cursor) {
            if (!compute_node(node, &max_rect, rect_origin_cursor)) {
                throw std::runtime_error("compute node failed");
            }
        } else if (!compute_node(node, &max_rect, *layout_cursor)) {
            throw std::runtime_error("compute node failed");
        }
        node->set_position(max_rect.position());
        node->set_width(max_rect.width());
        node->set_height(max_rect.height());
        if (!is_absolute && active_cursor.direction() == DocraftCursorDirection::kHorizontal) {
            // Advance cursor to the next column start, keeping a fixed horizontal gap.
            cursor.move_to(max_rect.anchors().top_right.x + kHorizontalSpacing_, max_rect.anchors().top_right.y);
        } else if (!is_absolute) {
            // Advance cursor down with a fixed vertical gap or per-node padding (whichever is larger).
            const float spacing = std::max(kVerticalSpacing_, node->padding());
            float next_y = max_rect.anchors().bottom_left.y - spacing;
            if (next_y < 0.0F) {
                next_y = 0.0F;
            }
            cursor.move_to(flow_origin_x, next_y);
        }
        return max_rect;
    }

    float DocraftLayoutEngine::compute_width(const std::shared_ptr<model::DocraftSection> &node) const {
        float margin_left = node->margin_left();
        float margin_right = node->margin_right();
        return context()->page_width() - (margin_left + margin_right);
    }

    void DocraftLayoutEngine::assign_page_owner_recursive(const std::shared_ptr<model::DocraftNode> &node,
                                                          int page) const {
        if (!node) {
            return;
        }
        node->set_page_owner(page);
        // Recursively assign page owner to children if it's a container
        if (auto container = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            for (const auto &child: container->children()) {
                assign_page_owner_recursive(child, page);
            }
        }
        //handle table children separately since they are not in the normal children list
        if (auto table = std::dynamic_pointer_cast<model::DocraftTable>(node)) {
            for (const auto &title: table->title_nodes()) {
                assign_page_owner_recursive(title, page);
            }
            for (const auto &title: table->htitle_nodes()) {
                assign_page_owner_recursive(title, page);
            }
            for (const auto &row: table->content_nodes()) {
                for (const auto &cell: row) {
                    assign_page_owner_recursive(cell, page);
                }
            }
        }
    }

    void DocraftLayoutEngine::compute_document_layout(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) {
        const Sections sections = split_sections(nodes);
        if (!sections.body) {
            throw std::runtime_error("Document must have a body section");
        }
        if (const auto &page_backend = context()->page_backend()) {
            page_backend->go_to_first_page();
        }
        const SectionPlan plan = build_section_plan(sections);
        if (plan.header_to_render) {
            layout_header_section(sections.header, plan.header_ratio);
        }
        if (plan.body_to_render) {
            layout_body_section(sections.body, sections.header, plan);
        }
        if (plan.footer_to_render) {
            layout_footer_section(sections.footer, sections.body, plan);
        }
    }

    DocraftLayoutEngine::Sections DocraftLayoutEngine::split_sections(
        const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) const {
        Sections sections;
        for (const auto &node: nodes) {
            if (const auto header_node = std::dynamic_pointer_cast<model::DocraftHeader>(node)) {
                sections.header = header_node;
            } else if (const auto body_node = std::dynamic_pointer_cast<model::DocraftBody>(node)) {
                sections.body = body_node;
            } else if (const auto footer_node = std::dynamic_pointer_cast<model::DocraftFooter>(node)) {
                sections.footer = footer_node;
            }
        }
        return sections;
    }

    DocraftLayoutEngine::SectionPlan DocraftLayoutEngine::build_section_plan(const Sections &sections) const {
        SectionPlan plan;
        const float base_header_ratio = context()->header_ratio();
        const float base_body_ratio = context()->body_ratio();
        const float base_footer_ratio = context()->footer_ratio();

        plan.header_ratio = sections.header ? base_header_ratio : 0.0F;
        plan.footer_ratio = sections.footer ? base_footer_ratio : 0.0F;
        plan.body_ratio = base_body_ratio;
        plan.header_to_render = sections.header && sections.header->visible();
        plan.body_to_render = sections.body && sections.body->visible();
        plan.footer_to_render = sections.footer && sections.footer->visible();

        if (!plan.header_to_render) {
            plan.body_ratio += base_header_ratio;
        }
        if (!plan.footer_to_render) {
            plan.body_ratio += base_footer_ratio;
        }
        return plan;
    }

    void DocraftLayoutEngine::layout_header_section(
        const std::shared_ptr<model::DocraftHeader> &header,
        const float header_ratio) {
        header->set_position({.x = header->margin_left(), .y = context()->page_height()});
        header->set_width(compute_width(header));
        header->set_height(context()->page_height() * header_ratio);
        context()->cursor().move_to(header->position().x, header->position().y);
        (void) compute_layout(header, context()->cursor());
        header->set_position({.x = header->margin_left(), .y = context()->page_height()});
        header->set_width(compute_width(header));
        header->set_height(context()->page_height() * header_ratio);
        assign_page_owner_recursive(header, -1); //always
    }

    void DocraftLayoutEngine::layout_body_section(
        const std::shared_ptr<model::DocraftBody> &body,
        const std::shared_ptr<model::DocraftHeader> &header,
        const SectionPlan &plan) {
        float body_start_y = context()->page_height();
        if (plan.header_to_render) {
            body_start_y = header->anchors().bottom_left.y;
        }
        const float body_height = context()->page_height() * plan.body_ratio;

        body->set_position({.x = body->margin_left(), .y = body_start_y});
        body->set_width(compute_width(body));
        body->set_height(body_height);
        const float footer_height = plan.footer_to_render ? context()->page_height() * plan.footer_ratio : 0.0F;
        const float body_bottom_y = (body_start_y - body_height)+ body->margin_bottom()+footer_height; // The y-coordinate of the bottom of the body content area, accounting for footer if present
        context()->set_current_rect_width(body->width());

        DocraftCursor body_cursor; //Use a custom cursor to not affect the main one
        body_cursor.move_to(body->position().x, body_start_y);

        int current_page = 1;
        const auto &page_backend = context()->page_backend();
        if (page_backend) {
            current_page = static_cast<int>(page_backend->current_page_number());
        }

        // Layout body children and handle pagination if needed.
        if (auto body_container = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(body)) {
            /**
            * This lambda recursively processes body children, handling pagination logic in place. It checks for explicit new page directives,
            * handles foreach loops by processing their children in place, and checks for overflow after layout to determine if a new page is needed. By using a lambda,
            * we maintain access to local variables like body_cursor and current_page without needing to pass them through multiple function parameters.
            * @param child The current child node being processed.
            * @param index_ptr Optional pointer to the current index in the container's child list, used for inserting new nodes if a table is split across pages. If nullptr,
            * it indicates that we're processing children of a foreach loop and should not modify the container's child list directly.
            */
            std::function<void(const std::shared_ptr<model::DocraftNode> &, std::size_t *)> process_child;
            // Use a lambda to allow recursion while maintaining access to local variables like body_cursor and current_page
            process_child = [&](const std::shared_ptr<model::DocraftNode> &child,
                                const std::size_t *index_ptr) {
                if (!child) {
                    return;
                }
                if (std::dynamic_pointer_cast<model::DocraftNewPage>(child)) {
                    // Handle explicit new page directive
                    if (page_backend) {
                        page_backend->add_new_page();
                        ++current_page;
                    }
                    // Move cursor to top of new page
                    body_cursor.reset_direction();
                    body_cursor.move_to(body->position().x, body_start_y);
                    return;
                }
                // Handle foreach loops by processing their children in place, ensuring they inherit the correct page owner and layout context
                if (auto foreach_node = std::dynamic_pointer_cast<model::DocraftForeach>(child)) {
                    foreach_node->set_page_owner(-1);
                    const auto &foreach_children = foreach_node->children();
                    for (std::size_t i = 0; i < foreach_children.size(); ++i) {
                        const auto &foreach_child = foreach_children[i];
                        if (std::dynamic_pointer_cast<model::DocraftNewPage>(foreach_child) &&
                            i + 1 == foreach_children.size()) {
                            // Skip trailing NewPage to avoid an extra blank page.
                            continue;
                        }
                        process_child(foreach_child, nullptr);
                        // Process foreach children with the same lambda, but pass nullptr for index since we're not modifying the container's child list here.
                    }
                    return;
                }

                assign_page_owner_recursive(child, current_page);
                // Ensure layout sees the correct page owner, especially for new nodes created by foreach processing.
                DocraftCursor child_start_cursor = body_cursor;
                const auto child_box = compute_layout(child, body_cursor); // Updates body_cursor
                const bool overflows_body =
                        child->position_mode() != model::DocraftPositionType::kAbsolute &&
                        child_box.anchors().bottom_left.y < body_bottom_y;
                // Check if the child overflows the body section (only for non-absolute positioned nodes)
                if (!overflows_body) {
                    return;
                }
                //Handle table splitting across pages if the overflowing child is a table. If it can be split, we split it and insert the remainder
                //back into the container to be processed on the next page. If it can't be split, we just move it to the next page.
                if (auto table = std::dynamic_pointer_cast<model::DocraftTable>(child)) {
                    const auto total_rows = static_cast<std::size_t>(table->rows());
                    const auto fit_rows = table->orientation() == model::LayoutOrientation::kVertical
                                                     ? count_rows_fit_vertical(*table, body_bottom_y)
                                                     : count_rows_fit_horizontal(*table, body_bottom_y);
                    if (fit_rows > 0 && fit_rows < total_rows) {
                        // Split the table and insert the remainder back into the container for the next page.
                        // We only do this if at least one row can fit on the current page to avoid creating empty tables.
                        if (auto remainder = table->split_after_row(fit_rows, true)) {
                            if (index_ptr) {
                                // Only modify the container's child list if we're processing top-level children of the body, not foreach children.
                                body_container->insert_child(*index_ptr + 1, remainder); // Insert after original
                            }
                            body_cursor = child_start_cursor;
                            assign_page_owner_recursive(child, current_page);
                            // Re-assign page owner to the original table before re-layout in case it has changed due to foreach processing.
                            (void) compute_layout(child, body_cursor);
                            //Re-layout the original table with just the fitting rows on the current page.
                            if (page_backend) {
                                // Move to next page and update current_page for the remainder
                                page_backend->add_new_page();
                                ++current_page;
                            }
                            body_cursor.reset_direction();
                            body_cursor.move_to(body->position().x, body_start_y);
                            assign_page_owner_recursive(remainder, current_page);
                            return;
                        }
                    }
                }
                if (page_backend) {
                    // Move to next page and update current_page
                    page_backend->add_new_page();
                    ++current_page;
                }
                body_cursor.reset_direction();
                body_cursor.move_to(body->position().x, body_start_y); // Move cursor to top for new page
                assign_page_owner_recursive(child, current_page); // Update page owner before re-layout
                (void) compute_layout(child, body_cursor); // Re-layout the child on the new page
            };

            // Start processing body children with the lambda, passing the index pointer for top-level children to allow table splitting logic to modify the container's child list if needed.
            std::size_t index = 0;
            while (index < body_container->children().size()) {
                auto child = body_container->children()[index];
                process_child(child, &index);
                ++index;
            }
        }

        body->set_position({.x = body->margin_left(), .y = body_start_y});
        body->set_width(compute_width(body));
        body->set_height(body_height);
    }

    void DocraftLayoutEngine::layout_footer_section(
        const std::shared_ptr<model::DocraftFooter> &footer,
        const std::shared_ptr<model::DocraftBody> &body,
        const SectionPlan &plan) {
        float footer_start_y = 0.0F;
        if (plan.body_to_render) {
            footer_start_y = body->anchors().bottom_left.y;
        }
        footer->set_position({.x = footer->margin_left(), .y = footer_start_y});
        footer->set_width(compute_width(footer));
        footer->set_height(context()->page_height() * plan.footer_ratio);
        context()->cursor().move_to(footer->position().x, footer_start_y);
        compute_layout(footer, context()->cursor());
        footer->set_position({.x = footer->margin_left(), .y = footer_start_y});
        footer->set_width(compute_width(footer));
        footer->set_height(context()->page_height() * plan.footer_ratio);
        assign_page_owner_recursive(footer, -1); //always
    }
} // docraft

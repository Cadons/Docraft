/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "docraft_layout_engine_impl.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "docraft/backend/docraft_page_rendering_backend.h"
#include "docraft/docraft_document_context.h"
#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/layout/handler/docraft_basic_layout_handler.h"
#include "docraft/layout/handler/docraft_layout_blank_line.h"
#include "docraft/layout/handler/docraft_layout_handler.h"
#include "docraft/layout/handler/docraft_layout_list_handler.h"
#include "docraft/layout/handler/docraft_layout_table_handler.h"
#include "docraft/layout/handler/docraft_layout_text_handler.h"
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_footer.h"
#include "docraft/model/docraft_foreach.h"
#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_list.h"
#include "docraft/model/docraft_new_page.h"
#include "docraft/model/docraft_table.h"

namespace {
    std::size_t count_rows_fit(const docraft::model::DocraftTable &table, const float body_bottom_y) {
        std::size_t fit = 0;
        auto grid = table.content_nodes();
        for (const auto &row: grid) {
            float row_bottom = std::numeric_limits<float>::infinity();
            bool found = false;
            for (const auto &cell: row) {
                if (cell) {
                    // Use the lowest bottom among all row cells to avoid optimistic fit decisions.
                    row_bottom = std::min(row_bottom, cell->anchors().bottom_left.y);
                    found = true;
                }
            }
            if (!found) {
                return fit;
            }
            //if the row bottom is above the body bottom, it fits, otherwise it doesn't
            if (row_bottom <= body_bottom_y) {
                return fit;
            }
            ++fit;
        }
        return fit;
    }

}

namespace docraft::layout {
    // Layout engine workflow:
    // 1) Measure constraints and establish the active cursor/available width for the current node.
    // 2) Layout children (if any), collecting child boxes.
    // 3) Compute and place the node itself, then advance cursor and handle page ownership/pagination.

    DocraftLayoutEngine::Impl::Impl(std::shared_ptr<DocraftDocumentContext> context, const bool reset_cursor)
        : context_(std::move(context)) {
        configure_handlers();
        if (reset_cursor && context_) {
            auto &layout_service = context_->edit_layout();
            layout_service.cursor().move_to(0, layout_service.page_height());
        }
    }

    const std::shared_ptr<DocraftDocumentContext> &DocraftLayoutEngine::Impl::context() const {
        return context_;
    }

    model::DocraftTransform DocraftLayoutEngine::Impl::compute_max_rect(
        const std::vector<model::DocraftTransform> &boxes) {
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

        const float width = max_x - min_x;
        const float height = max_y - min_y;

        return model::DocraftTransform({.x = min_x, .y = max_y}, width, height);
    }

    model::DocraftTransform DocraftLayoutEngine::Impl::compute_layout(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node->visible()) {
            return model::DocraftTransform{};
        }
        auto &cursor = context_->edit_layout().cursor();
        return compute_layout(node, cursor);
    }

    model::DocraftTransform DocraftLayoutEngine::Impl::compute_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                                      DocraftCursor &cursor) {
        if (!node->visible()) {
            return model::DocraftTransform{};
        }

        // Phase 1: derive measurement constraints and choose the cursor used by this node.
        std::vector<model::DocraftTransform> child_boxes;
        auto &layout_service = context_->edit_layout();
        float max_width = layout_service.available_space();
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
                rect_container->set_position({.x = rect_cursor.x(), .y = rect_cursor.y()});
                rect_origin_cursor = rect_cursor;
                rect_uses_origin_cursor = true;
                local_node_cursor = rect_cursor;
                layout_cursor = &local_node_cursor;
                if (rect_container->width() > 0.0F) {
                    max_width = rect_container->width();
                }
                layout_service.set_current_rect_width(max_width);
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
            active_cursor.move_to(base_x, active_cursor.y() - top_margin - padding);
            if (section_node->width() > 0.0F) {
                max_width = section_node->width();
            } else {
                max_width = max_width - left_margin - right_margin;
            }
            layout_service.set_current_rect_width(max_width);
            if (section_node->height() > 0.0F) {
                section_content_bottom = section_node->position().y - section_node->height() +
                                         section_node->margin_bottom() + padding;
                section_has_bounds = true;
            }
        }
        if (std::dynamic_pointer_cast<model::DocraftLayout>(node)) {
            auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(node);
            if (layout_node->orientation() == model::LayoutOrientation::kHorizontal) {
                layout_cursor->push_direction(DocraftCursorDirection::kHorizontal);
            } else {
                layout_cursor->push_direction(DocraftCursorDirection::kVertical);
            }
        }

        // Phase 2: layout children (lists/containers) and collect their boxes.
        if (auto list_node = std::dynamic_pointer_cast<model::DocraftList>(node)) {
            if (!list_handler_) {
                throw docraft::exception::LayoutConfigurationException("DocraftLayoutListHandler not configured");
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
            normalize_child_weights(*container_node);
            const float saved_available_space = layout_service.available_space();
            const bool is_horizontal = (layout_cursor->direction() == DocraftCursorDirection::kHorizontal);
            if (is_horizontal) {
                layout_children_horizontal(container_node, node->z_index(), max_width,
                                           *layout_cursor, child_boxes,
                                           section_has_bounds, section_content_bottom);
            } else {
                layout_children_vertical(container_node, node->z_index(), max_width,
                                         *layout_cursor, child_boxes,
                                         section_has_bounds, section_content_bottom);
            }
            layout_service.set_current_rect_width(saved_available_space);
        }

        auto max_rect = compute_max_rect(child_boxes);

        // Phase 3: compute this node box, place it, then update flow cursor for the next sibling.
        if (rect_uses_origin_cursor) {
            if (!compute_node(node, &max_rect, rect_origin_cursor)) {
                throw docraft::exception::LayoutException("compute node failed");
            }
        } else if (!compute_node(node, &max_rect, *layout_cursor)) {
            throw docraft::exception::LayoutException("compute node failed");
        }
        node->set_position(max_rect.position());
        node->set_width(max_rect.width());
        node->set_height(max_rect.height());
        if (!is_absolute && active_cursor.direction() == DocraftCursorDirection::kHorizontal) {
            cursor.move_to(max_rect.anchors().top_right.x + kHorizontalSpacing, max_rect.anchors().top_right.y);
        } else if (!is_absolute) {
            const float spacing = std::max(kVerticalSpacing, node->padding());
            float next_y = max_rect.anchors().bottom_left.y - spacing;
            if (next_y < 0.0F) {
                next_y = 0.0F;
            }
            cursor.move_to(flow_origin_x, next_y);
        }
        return max_rect;
    }

    void DocraftLayoutEngine::Impl::compute_document_layout(
        const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) {
        // Document pass: split header/body/footer, layout each visible section, then paginate body content.
        const Sections sections = split_sections(nodes);
        if (!sections.body) {
            throw docraft::exception::DocumentStateException("Document must have a body section");
        }
        if (const auto page_backend = context_->edit_rendering().edit_page_rendering()) {
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

    void DocraftLayoutEngine::Impl::configure_handlers() {
        list_handler_ = nullptr;
        handlers_.clear();
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTextHandler>(context_));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutHandler>(context_));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTableHandler>(context_));
        // dispatches internally to horizontal/vertical sub-handlers
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutBlankLine>(context_));
        auto list_handler = std::make_unique<handler::DocraftLayoutListHandler>(context_);
        list_handler_ = list_handler.get();
        handlers_.emplace_back(std::move(list_handler));
        handlers_.emplace_back(std::make_unique<handler::DocraftBasicLayoutHandler>(context_));
    }

    bool DocraftLayoutEngine::Impl::compute_node(const std::shared_ptr<model::DocraftNode> &node,
                                                 model::DocraftTransform *box,
                                                 DocraftCursor &cursor) const {
        for (const auto &handler: handlers_) {
            if (handler->handle(node, box, cursor)) {
                return true;
            }
        }
        return false;
    }

    float DocraftLayoutEngine::Impl::compute_width(const std::shared_ptr<model::DocraftSection> &node) const {
        const float margin_left = node->margin_left();
        const float margin_right = node->margin_right();
        return context_->layout().page_width() - (margin_left + margin_right);
    }

    void DocraftLayoutEngine::Impl::assign_page_owner_recursive(const std::shared_ptr<model::DocraftNode> &node,
                                                                const int page) const {
        if (!node) {
            return;
        }
        node->set_page_owner(page);
        if (auto container = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            for (const auto &child: container->children()) {
                assign_page_owner_recursive(child, page);
            }
        }
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

    // ── Layout orientation helpers ──────────────────────────────────────────────

    void DocraftLayoutEngine::Impl::normalize_child_weights(
        model::DocraftChildrenContainerNode &container) {
        if (container.children().empty()) {
            return;
        }
        bool has_variable = false;
        for (const auto &child: container.children()) {
            if (child->weight() == -1.0F) {
                has_variable = true;
                break;
            }
        }
        if (has_variable) {
            const float equal_weight = 1.0F / static_cast<float>(container.children().size());
            for (const auto &child: container.children()) {
                child->set_weight(equal_weight);
            }
        }
    }

    float DocraftLayoutEngine::Impl::compute_horizontal_available_width(
        const float max_width, const std::size_t child_count) {
        if (child_count <= 1) {
            return max_width;
        }
        const float total_spacing = kHorizontalSpacing * static_cast<float>(child_count - 1);
        return std::max(0.0F, max_width - total_spacing);
    }

    void DocraftLayoutEngine::Impl::process_child_layout(
        const std::shared_ptr<model::DocraftNode> &child,
        DocraftCursor &cursor,
        std::vector<model::DocraftTransform> &out_boxes,
        const bool section_has_bounds,
        const float section_content_bottom) {
        const auto box = compute_layout(child, cursor);
        out_boxes.emplace_back(box);
        if (section_has_bounds && cursor.y() < section_content_bottom) {
            cursor.set_y(section_content_bottom);
        }
    }

    void DocraftLayoutEngine::Impl::layout_children_vertical(
        const std::shared_ptr<model::DocraftChildrenContainerNode> &container,
        const int parent_z_index,
        const float max_width,
        DocraftCursor &cursor,
        std::vector<model::DocraftTransform> &out_boxes,
        const bool section_has_bounds,
        const float section_content_bottom) {
        auto &layout_service = context_->edit_layout();
        for (const auto &child: container->children()) {
            if (child->z_index() != parent_z_index) {
                continue;
            }
            layout_service.set_current_rect_width(max_width);
            process_child_layout(child, cursor, out_boxes, section_has_bounds, section_content_bottom);
        }
    }

    void DocraftLayoutEngine::Impl::layout_children_horizontal(
        const std::shared_ptr<model::DocraftChildrenContainerNode> &container,
        const int parent_z_index,
        const float max_width,
        DocraftCursor &cursor,
        std::vector<model::DocraftTransform> &out_boxes,
        const bool section_has_bounds,
        const float section_content_bottom) {
        auto &layout_service = context_->edit_layout();
        const float available = compute_horizontal_available_width(max_width, container->children().size());
        for (const auto &child: container->children()) {
            if (child->z_index() != parent_z_index) {
                continue;
            }
            const float child_width = available * child->weight();
            layout_service.set_current_rect_width(child_width);
            const float start_x = cursor.x();
            const float start_y = cursor.y();
            process_child_layout(child, cursor, out_boxes, section_has_bounds, section_content_bottom);
            // Advance the cursor to the next horizontal slot.
            cursor.move_to(start_x + child_width + kHorizontalSpacing, start_y);
        }
    }

    // ───────────────────────────────────────────────────────────────────────────

    void DocraftLayoutEngine::Impl::advance_to_next_body_page(BodyLayoutState &state) {
        if (state.page_backend) {
            state.page_backend->add_new_page();
            ++state.current_page;
        }
        state.body_cursor.reset_direction();
        state.body_cursor.move_to(state.body->position().x, state.body_start_y);
    }

    bool DocraftLayoutEngine::Impl::handle_table_overflow_on_body_page(const std::shared_ptr<model::DocraftNode> &child,
                                                                       const std::size_t *index_ptr,
                                                                       const DocraftCursor &child_start_cursor,
                                                                       BodyLayoutState &state) {
        auto table = std::dynamic_pointer_cast<model::DocraftTable>(child);
        if (!table) {
            return false;
        }

        const auto total_rows = static_cast<std::size_t>(table->rows());
        auto footer = context()->navigation().footer();
        const auto fit_rows = count_rows_fit(*table, state.body_bottom_y);

        if (fit_rows == 0 || fit_rows >= total_rows) {
            return false;
        }

        auto remainder = table->split_after_row(fit_rows, true);
        if (!remainder) {
            return false;
        }

        if (index_ptr) {
            state.body_container->insert_child(*index_ptr + 1, remainder);
        }

        state.body_cursor = child_start_cursor;
        assign_page_owner_recursive(child, state.current_page);
        (void) compute_layout(child, state.body_cursor);

        advance_to_next_body_page(state);
        assign_page_owner_recursive(remainder, state.current_page);
        return true;
    }

    void DocraftLayoutEngine::Impl::process_body_child_with_pagination(const std::shared_ptr<model::DocraftNode> &child,
                                                                       const std::size_t *index_ptr,
                                                                       BodyLayoutState &state) {
        if (!child) {
            return;
        }

        if (std::dynamic_pointer_cast<model::DocraftNewPage>(child)) {
            advance_to_next_body_page(state);
            return;
        }

        if (auto foreach_node = std::dynamic_pointer_cast<model::DocraftForeach>(child)) {
            foreach_node->set_page_owner(-1);
            const auto &foreach_children = foreach_node->children();
            for (std::size_t i = 0; i < foreach_children.size(); ++i) {
                const auto &foreach_child = foreach_children[i];
                if (std::dynamic_pointer_cast<model::DocraftNewPage>(foreach_child) && i + 1 ==
                    foreach_children.size()) {
                    continue;
                }
                process_body_child_with_pagination(foreach_child, nullptr, state);
            }
            return;
        }

        assign_page_owner_recursive(child, state.current_page);
        DocraftCursor child_start_cursor = state.body_cursor;
        const auto child_box = compute_layout(child, state.body_cursor);
        const bool overflows_body =
                child->position_mode() != model::DocraftPositionType::kAbsolute &&
                child_box.anchors().bottom_left.y < state.body_bottom_y;

        if (!overflows_body) {
            return;
        }

        if (handle_table_overflow_on_body_page(child, index_ptr, child_start_cursor, state)) {
            return;
        }

        if (const bool starts_at_fresh_page_top = std::abs(child_start_cursor.y() - state.body_start_y) <= 0.01F) {
            // Node cannot fit even on a fresh page; avoid generating pointless blank pages.
            return;
        }

        advance_to_next_body_page(state);
        assign_page_owner_recursive(child, state.current_page);
        (void) compute_layout(child, state.body_cursor);
    }

    DocraftLayoutEngine::Impl::Sections DocraftLayoutEngine::Impl::split_sections(
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

    DocraftLayoutEngine::Impl::SectionPlan
    DocraftLayoutEngine::Impl::build_section_plan(const Sections &sections) const {
        SectionPlan plan;
        const auto &navigation_service = context_->navigation();
        const float base_header_ratio = navigation_service.header_ratio();
        const float base_body_ratio = navigation_service.body_ratio();
        const float base_footer_ratio = navigation_service.footer_ratio();

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

    void DocraftLayoutEngine::Impl::layout_header_section(const std::shared_ptr<model::DocraftHeader> &header,
                                                          const float header_ratio) {
        auto &layout_service = context_->edit_layout();
        auto &layout_cursor = layout_service.cursor();
        const float page_height = layout_service.page_height();
        header->set_position({.x = header->margin_left(), .y = page_height});
        header->set_width(compute_width(header));
        header->set_height(page_height * header_ratio);
        layout_cursor.move_to(header->position().x, header->position().y);
        (void) compute_layout(header, layout_cursor);
        header->set_position({.x = header->margin_left(), .y = page_height});
        header->set_width(compute_width(header));
        header->set_height(page_height * header_ratio);
        assign_page_owner_recursive(header, -1);
    }

    void DocraftLayoutEngine::Impl::layout_body_section(const std::shared_ptr<model::DocraftBody> &body,
                                                        const std::shared_ptr<model::DocraftHeader> &header,
                                                        const SectionPlan &plan) {
        auto &layout_service = context_->edit_layout();
        auto &rendering_service = context_->edit_rendering();
        const float page_height = layout_service.page_height();
        float body_start_y = page_height;
        if (plan.header_to_render) {
            body_start_y = header->anchors().bottom_left.y;
        }
        const float body_height = page_height * plan.body_ratio;

        body->set_position({.x = body->margin_left(), .y = body_start_y});
        body->set_width(compute_width(body));
        body->set_height(body_height);
        const float footer_height = plan.footer_to_render ? page_height * plan.footer_ratio : 0.0F;
        const float body_bottom_y = (body_start_y - body_height) + body->margin_bottom() + footer_height;

        DocraftCursor body_cursor;
        body_cursor.move_to(body->position().x, body_start_y);

        int current_page = 1;
        const auto page_backend = rendering_service.edit_page_rendering();
        if (page_backend) {
            current_page = static_cast<int>(page_backend->current_page_number());
        }

        if (auto body_container = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(body)) {
            BodyLayoutState body_layout_state{
                .body = body,
                .body_container = body_container,
                .page_backend = page_backend,
                .body_cursor = body_cursor,
                .body_start_y = body_start_y,
                .body_bottom_y = body_bottom_y,
                .current_page = current_page
            };

            std::size_t index = 0;
            while (index < body_container->children().size()) {
                auto child = body_container->children()[index];
                process_body_child_with_pagination(child, &index, body_layout_state);
                ++index;
            }
        }

        body->set_position({.x = body->margin_left(), .y = body_start_y});
        body->set_width(compute_width(body));
        body->set_height(body_height);
    }

    void DocraftLayoutEngine::Impl::layout_footer_section(const std::shared_ptr<model::DocraftFooter> &footer,
                                                          const std::shared_ptr<model::DocraftBody> &body,
                                                          const SectionPlan &plan) {
        auto &layout_service = context_->edit_layout();
        auto &layout_cursor = layout_service.cursor();
        float footer_start_y = 0.0F;
        if (plan.body_to_render) {
            footer_start_y = body->anchors().bottom_left.y;
        }
        footer->set_position({.x = footer->margin_left(), .y = footer_start_y});
        footer->set_width(compute_width(footer));
        footer->set_height(layout_service.page_height() * plan.footer_ratio);
        layout_cursor.move_to(footer->position().x, footer_start_y);
        (void) compute_layout(footer, layout_cursor);
        footer->set_position({.x = footer->margin_left(), .y = footer_start_y});
        footer->set_width(compute_width(footer));
        footer->set_height(layout_service.page_height() * plan.footer_ratio);
        assign_page_owner_recursive(footer, -1);
    }
} // namespace docraft::layout


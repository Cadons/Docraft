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

    /**
     * @brief Constructs the implementation, configures handlers, and optionally resets the cursor.
     */
    DocraftLayoutEngine::Impl::Impl(std::shared_ptr<DocraftDocumentContext> context, const bool reset_cursor)
        : context_(std::move(context)) {
        configure_handlers();
        if (reset_cursor && context_) {
            auto &layout_service = context_->edit_layout();
            layout_service.cursor().move_to(0, layout_service.page_height());
        }
    }

    /**
     * @brief Returns the shared document context used by the layout engine.
     */
    const std::shared_ptr<DocraftDocumentContext> &DocraftLayoutEngine::Impl::context() const {
        return context_;
    }

    /**
     * @brief Computes the minimal bounding rectangle that encloses a set of boxes.
     */
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

    /**
     * @brief Lays out a node using the default layout cursor from the document context.
     */
    model::DocraftTransform DocraftLayoutEngine::Impl::compute_layout(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node->visible()) {
            return model::DocraftTransform{};
        }
        auto &cursor = context_->edit_layout().cursor();
        return compute_layout(node, cursor);
    }

    /**
     * @brief Lays out a node starting from an explicit cursor position.
     */
    /**
     * @brief Lays out a node starting from an explicit cursor position.
     */
    model::DocraftTransform DocraftLayoutEngine::Impl::compute_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                                      DocraftCursor &cursor) {
        if (!node->visible()) {
            return model::DocraftTransform{};
        }

        auto state = prepare_layout_state(node, cursor);
        layout_node_children(node, state);
        auto max_rect = compute_max_rect(state.child_boxes);
        return finalize_layout(node, cursor, state, max_rect);
    }

    /**
     * @brief Lays out the document sections in Header/Body/Footer order.
     */
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

    /**
     * @brief Installs the concrete layout handlers in priority order.
     */
    void DocraftLayoutEngine::Impl::configure_handlers() {
        list_handler_ = nullptr;
        handlers_.clear();
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTextHandler>(context_));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutHandler>(context_));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTableHandler>(context_));
        // dispatches internally to horizontal/vertical sub-handlers
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutBlankLine>(context_));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutListHandler>(context_));
        list_handler_ = static_cast<handler::DocraftLayoutListHandler *>(handlers_.back().get());
        handlers_.emplace_back(std::make_unique<handler::DocraftBasicLayoutHandler>(context_));
    }

    /**
     * @brief Dispatches the current node through the handler chain.
     */
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

    void DocraftLayoutEngine::Impl::setup_container_cursor_state(const std::shared_ptr<model::DocraftNode> &node,
                                                                 LayoutComputationState &state) {
        if (auto rect_container = std::dynamic_pointer_cast<model::DocraftRectangle>(node)) {
            auto &layout_service = context_->edit_layout();
            if (!std::dynamic_pointer_cast<model::DocraftSection>(node) && !rect_container->children().empty()) {
                DocraftCursor rect_cursor = state.node_cursor;
                if (rect_container->position_mode() == model::DocraftPositionType::kAbsolute) {
                    rect_cursor.move_to(state.flow_origin_x + rect_container->position().x,
                                        state.flow_origin_y - rect_container->position().y);
                } else {
                    rect_cursor.move_to(state.node_cursor.x(), state.node_cursor.y());
                }
                rect_container->set_position({.x = rect_cursor.x(), .y = rect_cursor.y()});
                state.box_origin_cursor = rect_cursor;
                state.use_box_origin_cursor = true;
                state.child_cursor = rect_cursor;
                // Rectangle children flow from a local cursor, while the node box keeps its own origin.
                state.selected_cursor = &state.child_cursor;
                if (rect_container->width() > 0.0F) {
                    state.max_width = rect_container->width();
                }
                layout_service.set_current_rect_width(state.max_width);
            }
        }
    }

    void DocraftLayoutEngine::Impl::setup_section_bounds_state(const std::shared_ptr<model::DocraftNode> &node,
                                                               LayoutComputationState &state) {
        if (auto section = std::dynamic_pointer_cast<model::DocraftSection>(node)) {
            auto &layout_service = context_->edit_layout();
            const float left_margin = section->margin_left();
            const float right_margin = section->margin_right();
            const float top_margin = section->margin_top();
            const float padding = std::max(0.0F, section->padding());
            float base_x = section->position().x;
            if (base_x == 0.0F && left_margin > 0.0F) {
                base_x = left_margin;
            }
            state.node_cursor.move_to(base_x, state.node_cursor.y() - top_margin - padding);
            if (section->width() > 0.0F) {
                state.max_width = section->width();
            } else {
                state.max_width = state.max_width - left_margin - right_margin;
            }
            layout_service.set_current_rect_width(state.max_width);
            if (section->height() > 0.0F) {
                state.section_content_bottom = section->position().y - section->height() + section->margin_bottom() +
                                               padding;
                state.section_has_bounds = true;
            }
        }
    }

    void DocraftLayoutEngine::Impl::setup_layout_direction_state(const std::shared_ptr<model::DocraftNode> &node,
                                                                 LayoutComputationState &state) {
        if (auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(node)) {
            if (layout_node->orientation() == model::LayoutOrientation::kHorizontal) {
                state.selected_cursor->push_direction(DocraftCursorDirection::kHorizontal);
            } else {
                state.selected_cursor->push_direction(DocraftCursorDirection::kVertical);
            }
        }
    }

    /**
     * @brief Captures cursor, width, and positioning state before laying out a node.

    * Cursor model used by LayoutComputationState:
    * - node_cursor: base cursor for the current node (flow position or absolute anchor).
    * - child_cursor: optional scratch cursor for child layout when child flow must be isolated.
    * - box_origin_cursor: cursor used to place the node box when box anchoring differs
    *   from child flow (for example rectangle-like containers).
    * - selected_cursor: pointer to the cursor currently driving child traversal
    *   (usually node_cursor, sometimes child_cursor).
    *
    * Typical flow:
    * 1) prepare_layout_state() initializes node_cursor and defaults selected_cursor.
    * 2) setup_section_bounds_state()/setup_container_cursor_state() may adjust limits and
    *    switch selected_cursor to child_cursor.
    * 3) layout_node_children() consumes *selected_cursor.
    * 4) finalize_layout() computes the node box using box_origin_cursor only when needed;
    *    otherwise it uses *selected_cursor and then advances the external flow cursor.
    */
    DocraftLayoutEngine::Impl::LayoutComputationState DocraftLayoutEngine::Impl::prepare_layout_state(
        const std::shared_ptr<model::DocraftNode> &node, DocraftCursor &cursor) {
        //general state setup
        LayoutComputationState state;
        auto &layout_service = context_->edit_layout();
        state.max_width = layout_service.available_space();
        state.flow_origin_x = cursor.x();
        state.flow_origin_y = cursor.y();
        state.is_absolute = (node->position_mode() == model::DocraftPositionType::kAbsolute);
        // configure cursor and width based on node type
        DocraftCursor local_cursor = cursor;
        state.node_cursor = state.is_absolute ? local_cursor : cursor;
        if (state.is_absolute) {
            //if absolute positions set cursor to the node's position
            const float x = state.flow_origin_x + node->position().x; //absolute position is relative to the flow origin
            const float y = state.flow_origin_y + node->position().y; //absolute position is relative to the flow origin
            state.node_cursor.move_to(x, y);
        }
        // Default behavior: children consume node_cursor directly.
        state.child_cursor = state.node_cursor;
        state.box_origin_cursor = state.node_cursor;
        state.selected_cursor = &state.node_cursor;

        if (!state.is_absolute && (std::dynamic_pointer_cast<model::DocraftText>(node) ||
                                   std::dynamic_pointer_cast<model::DocraftList>(node))) {
            // Text/List measure with an isolated cursor to avoid mutating parent flow prematurely.
            state.child_cursor.move_to(state.node_cursor.x(), state.node_cursor.y());
            state.selected_cursor = &state.child_cursor;
        }
        // Apply section limits first, then optional container-origin cursor behavior.
        setup_section_bounds_state(node, state);
        setup_container_cursor_state(node, state);
        setup_layout_direction_state(node, state);
        return state;
    }

    /**
     * @brief Lays out the children of the current node using the prepared state.
     */
    /**
     * @brief Lays out the children of the current node using the prepared state.
     */
    void DocraftLayoutEngine::Impl::layout_node_children(const std::shared_ptr<model::DocraftNode> &node,
                                                         LayoutComputationState &state) {
        auto &layout_service = context_->edit_layout();
        auto &layout_cursor = *state.selected_cursor;

        if (auto list_node = std::dynamic_pointer_cast<model::DocraftList>(node)) {
            if (!list_handler_) {
                throw docraft::exception::LayoutConfigurationException("DocraftLayoutListHandler not configured");
            }
            DocraftCursor list_cursor = layout_cursor;
            list_handler_->compute_children(
                list_node,
                list_cursor,
                state.child_boxes,
                [this](const std::shared_ptr<model::DocraftNode> &child, DocraftCursor &child_cursor) { // NOLINT(readability-function-cognitive-complexity)
                    return compute_layout(child, child_cursor);
                },
                state.max_width);
            return;
        }

        if (auto container_node = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            normalize_child_weights(*container_node);
            const float saved_available_space = layout_service.available_space();
            const bool is_horizontal = (layout_cursor.direction() == DocraftCursorDirection::kHorizontal);
            if (is_horizontal) {
                layout_children_horizontal(container_node, node->z_index(), state.max_width,
                                           layout_cursor, state.child_boxes,
                                           state.section_has_bounds, state.section_content_bottom);
            } else {
                layout_children_vertical(container_node, node->z_index(), state.max_width,
                                         layout_cursor, state.child_boxes,
                                         state.section_has_bounds, state.section_content_bottom);
            }
            layout_service.set_current_rect_width(saved_available_space);
        }
    }

    /**
     * @brief Resolves the final box for the current node and advances the cursor.
     */
    model::DocraftTransform DocraftLayoutEngine::Impl::finalize_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                                       DocraftCursor &cursor,
                                                                       LayoutComputationState &state,
                                                                       model::DocraftTransform &max_rect) {
        auto &layout_cursor = *state.selected_cursor;
        // Some containers layout children with a local cursor but resolve their own box from box_origin_cursor.
        if (state.use_box_origin_cursor) {
            if (!compute_node(node, &max_rect, state.box_origin_cursor)) {
                throw docraft::exception::LayoutException("compute node failed");
            }
        } else if (!compute_node(node, &max_rect, layout_cursor)) {
            throw docraft::exception::LayoutException("compute node failed");
        }

        node->set_position(max_rect.position());
        node->set_width(max_rect.width());
        node->set_height(max_rect.height());
        if (!state.is_absolute && layout_cursor.direction() == DocraftCursorDirection::kHorizontal) {
            cursor.move_to(max_rect.anchors().top_right.x + kHorizontalSpacing, max_rect.anchors().top_right.y);
        } else if (!state.is_absolute) {
            const float spacing = std::max(kVerticalSpacing, node->padding());
            const float next_y = std::max(0.0F, max_rect.anchors().bottom_left.y - spacing);
            cursor.move_to(state.flow_origin_x, next_y);
        }
        return max_rect;
    }

    /**
     * @brief Computes the usable width for a section-like node.
     */
    float DocraftLayoutEngine::Impl::compute_width(const std::shared_ptr<model::DocraftSection> &node) const {
        const float margin_left = node->margin_left();
        const float margin_right = node->margin_right();
        return context_->layout().page_width() - (margin_left + margin_right);
    }

    /**
     * @brief Propagates page ownership recursively through a node subtree.
     */
    /**
     * @brief Propagates page ownership recursively through a node subtree.
     */
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

    /**
     * @brief Normalises child weights when a layout uses the sentinel variable-weight value.
     */
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

    /**
     * @brief Returns the width available to horizontal children after spacing.
     */
    float DocraftLayoutEngine::Impl::compute_horizontal_available_width(
        const float max_width, const std::size_t child_count) {
        if (child_count <= 1) {
            return max_width;
        }
        const float total_spacing = kHorizontalSpacing * static_cast<float>(child_count - 1);
        return std::max(0.0F, max_width - total_spacing);
    }

    /**
     * @brief Lays out a child, stores its box, and clamps the cursor to the section bottom.
     */
    /**
     * @brief Lays out a child, stores its box, and clamps the cursor to the section bottom.
     */
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

    /**
     * @brief Lays out children stacked vertically.
     */
    /**
     * @brief Lays out children stacked vertically.
     */
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

    /**
     * @brief Lays out children side by side using their assigned weights.
     */
    /**
     * @brief Lays out children side by side using their assigned weights.
     */
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

    /**
     * @brief Advances the body cursor to the top of the next page.
     */
    void DocraftLayoutEngine::Impl::advance_to_next_body_page(BodyLayoutState &state) {
        if (state.page_backend) {
            state.page_backend->add_new_page();
            ++state.current_page;
        }
        state.body_cursor.reset_direction();
        state.body_cursor.move_to(state.body->position().x, state.body_start_y);
    }

    /**
     * @brief Splits an overflowing table and relayouts the current fragment.
     */
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

    /**
     * @brief Processes a body child, handling page breaks, foreach expansion, and overflow.
     */
    /**
     * @brief Processes a body child, handling page breaks, foreach expansion, and overflow.
     */
    void DocraftLayoutEngine::Impl::process_body_child_with_pagination(const std::shared_ptr<model::DocraftNode> &child,
                                                                       const std::size_t *index_ptr,
                                                                       BodyLayoutState &state) { // NOLINT(readability-function-cognitive-complexity)
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

        if (std::abs(child_start_cursor.y() - state.body_start_y) <= 0.01F) {
            return;
        }

        advance_to_next_body_page(state);
        assign_page_owner_recursive(child, state.current_page);
        (void) compute_layout(child, state.body_cursor);
    }

    /**
     * @brief Extracts the top-level Header/Body/Footer sections from the node list.
     */
    DocraftLayoutEngine::Impl::Sections DocraftLayoutEngine::Impl::split_sections(
        const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) {
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

    /**
     * @brief Builds the section visibility and ratio plan used during document layout.
     */
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

    /**
     * @brief Lays out the header section and re-anchors it to the page top.
     */
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

    /**
     * @brief Lays out the body section and paginates its children.
     */
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

    /**
     * @brief Lays out the footer section and re-anchors it to the page bottom area.
     */
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


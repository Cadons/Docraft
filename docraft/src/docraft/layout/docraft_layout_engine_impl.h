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

#pragma once

#include <memory>
#include <vector>

#include "docraft/generic/chain_of_responsibility_handler.h"
#include "docraft/layout/docraft_layout_engine.h"

namespace docraft {
    class DocraftDocumentContext;

    namespace backend {
        class IDocraftPageRenderingBackend;
    }

    namespace model {
        class DocraftBody;
        class DocraftChildrenContainerNode;
        class DocraftFooter;
        class DocraftHeader;
        class DocraftNode;
        class DocraftSection;
    }

    namespace layout::handler {
        class DocraftLayoutListHandler;
    }
}

namespace docraft::layout {
    class DocraftLayoutEngine::Impl {
    public:
        /**
         * @brief Builds the layout engine implementation and configures the handler chain.
         *
         * @param context Shared document context used by layout and pagination.
         * @param reset_cursor When true, resets the layout cursor to the top of the page.
         */
        explicit Impl(std::shared_ptr<DocraftDocumentContext> context, bool reset_cursor);

        /**
         * @brief Returns the shared document context used by the engine.
         */
        const std::shared_ptr<DocraftDocumentContext> &context() const;

        /**
         * @brief Computes the bounding rectangle that encloses the given boxes.
         */
        static model::DocraftTransform compute_max_rect(const std::vector<model::DocraftTransform> &boxes);

        /**
         * @brief Lays out a node using the engine's default cursor.
         */
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode> &node);

        /**
         * @brief Lays out a node starting from the provided cursor.
         */
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode> &node, DocraftCursor &cursor);

        /**
         * @brief Computes the layout for the full document sections.
         */
        void compute_document_layout(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes);

    private:
        static constexpr float kHorizontalSpacing = 4.0F;
        static constexpr float kVerticalSpacing = 4.0F;

        struct Sections {
            std::shared_ptr<model::DocraftHeader> header;
            std::shared_ptr<model::DocraftBody> body;
            std::shared_ptr<model::DocraftFooter> footer;
        };

        struct SectionPlan {
            float header_ratio = 0.0F;
            float body_ratio = 0.0F;
            float footer_ratio = 0.0F;
            bool header_to_render = false;
            bool body_to_render = false;
            bool footer_to_render = false;
        };

        struct BodyLayoutState {
            std::shared_ptr<model::DocraftBody> body;
            std::shared_ptr<model::DocraftChildrenContainerNode> body_container;
            std::shared_ptr<backend::IDocraftPageRenderingBackend> page_backend;
            DocraftCursor body_cursor;
            float body_start_y = 0.0F;
            float body_bottom_y = 0.0F;
            int current_page = 1;
        };

        struct LayoutComputationState {
            std::vector<model::DocraftTransform> child_boxes;
            float max_width = 0.0F;
            float flow_origin_x = 0.0F;
            float flow_origin_y = 0.0F;
            bool is_absolute = false;
            bool use_box_origin_cursor = false;
            bool section_has_bounds = false;
            float section_content_bottom = 0.0F;
            // Base cursor for the current node (flow cursor or absolute-position anchor).
            DocraftCursor node_cursor;
            // Scratch cursor used when children need isolated progression from node_cursor.
            DocraftCursor child_cursor;
            // Origin used to place the node box when container anchoring differs from child flow.
            DocraftCursor box_origin_cursor;
            // Pointer to the cursor currently driving child layout (node_cursor or child_cursor).
            DocraftCursor *selected_cursor = nullptr;

            LayoutComputationState() = default;

            LayoutComputationState(const LayoutComputationState &) = default;

            LayoutComputationState &operator=(const LayoutComputationState &) = default;

            LayoutComputationState(LayoutComputationState &&) noexcept = default;

            LayoutComputationState &operator=(LayoutComputationState &&) noexcept = default;
        };

        /**
         * @brief Installs the concrete layout handlers in priority order.
         */
        void configure_handlers();

        /**
         * @brief Invokes the handler chain for the current node.
         */
        bool compute_node(const std::shared_ptr<model::DocraftNode> &node,
                          model::DocraftTransform *box,
                          DocraftCursor &cursor) const;

        /**
         * @brief Computes the usable width for a section-like node.
         */
        float compute_width(const std::shared_ptr<model::DocraftSection> &node) const;

        /**
         * @brief Propagates the page owner recursively to a node subtree.
         */
        void assign_page_owner_recursive(const std::shared_ptr<model::DocraftNode> &node, int page) const;

        // ── Layout orientation helpers ──────────────────────────────────────────

        /**
         * @brief Normalises child weights: when any child carries the sentinel
         *        value -1 every child is assigned an equal share (1/N).
         */
        static void normalize_child_weights(model::DocraftChildrenContainerNode &container);

        /**
         * @brief Returns the width available to horizontal children after
         *        subtracting the inter-child gap for all N-1 gaps.
         */
        static float compute_horizontal_available_width(float max_width, std::size_t child_count);

        /**
         * @brief Calls compute_layout for @p child, appends the resulting box to
         *        @p out_boxes, and clamps the cursor to the section bottom when
         *        applicable.  Used by both orientation helpers.
         */
        void process_child_layout(const std::shared_ptr<model::DocraftNode> &child,
                                  DocraftCursor &cursor,
                                  std::vector<model::DocraftTransform> &out_boxes,
                                  bool section_has_bounds,
                                  float section_content_bottom);

        /**
         * @brief Captures the cursor and width state needed to layout a node.
         */
        LayoutComputationState prepare_layout_state(const std::shared_ptr<model::DocraftNode> &node,
                                                    DocraftCursor &cursor);

        /**
         * @brief Lays out the children of the current node using the prepared state.
         */
        void layout_node_children(const std::shared_ptr<model::DocraftNode> &node,
                                  LayoutComputationState &state);

        /**
         * @brief Resolves the final box for the current node and advances the cursor.
         */
        model::DocraftTransform finalize_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                DocraftCursor &cursor,
                                                LayoutComputationState &state,
                                                model::DocraftTransform &max_rect);

        /**
         * @brief Lays out children stacked top-to-bottom (vertical orientation).
         *        Each child receives the full @p max_width.
         */
        void layout_children_vertical(const std::shared_ptr<model::DocraftChildrenContainerNode> &container,
                                      int parent_z_index,
                                      float max_width,
                                      DocraftCursor &cursor,
                                      std::vector<model::DocraftTransform> &out_boxes,
                                      bool section_has_bounds,
                                      float section_content_bottom);

        /**
         * @brief Lays out children side-by-side (horizontal orientation).
         *        Each child's width is proportional to its weight.  The cursor is
         *        advanced horizontally by the allocated slot width plus spacing
         *        after every child.
         */
        void layout_children_horizontal(const std::shared_ptr<model::DocraftChildrenContainerNode> &container,
                                        int parent_z_index,
                                        float max_width,
                                        DocraftCursor &cursor,
                                        std::vector<model::DocraftTransform> &out_boxes,
                                        bool section_has_bounds,
                                        float section_content_bottom);

        // ───────────────────────────────────────────────────────────────────────

        /**
         * @brief Advances the body cursor to the top of the next page.
         */
        static void advance_to_next_body_page(BodyLayoutState &state);

        /**
         * @brief Splits and re-layouts a table fragment when it overflows the body area.
         */
        bool handle_table_overflow_on_body_page(const std::shared_ptr<model::DocraftNode> &child,
                                                const std::size_t *index_ptr,
                                                const DocraftCursor &child_start_cursor,
                                                BodyLayoutState &state);

        /**
         * @brief Processes a body child, applying page breaks, foreach expansion and overflow rules.
         */
        void process_body_child_with_pagination(const std::shared_ptr<model::DocraftNode> &child,
                                                const std::size_t *index_ptr,
                                                BodyLayoutState &state);

        /**
         * @brief Extracts the top-level Header/Body/Footer sections from the document nodes.
         */
        static Sections split_sections(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes);

        /**
         * @brief Builds the section visibility and ratio plan used by document layout.
         */
        SectionPlan build_section_plan(const Sections &sections) const;

        /**
         * @brief Lays out the header section and re-anchors ownership to the document level.
         */
        void layout_header_section(const std::shared_ptr<model::DocraftHeader> &header, float header_ratio);

        /**
         * @brief Lays out the body section and applies pagination to its children.
         */
        void layout_body_section(const std::shared_ptr<model::DocraftBody> &body,
                                 const std::shared_ptr<model::DocraftHeader> &header,
                                 const SectionPlan &plan);

        /**
         * @brief Lays out the footer section and re-anchors ownership to the document level.
         */
        void layout_footer_section(const std::shared_ptr<model::DocraftFooter> &footer,
                                   const std::shared_ptr<model::DocraftBody> &body,
                                   const SectionPlan &plan);

        /**
         * @brief Applies section margins/limits and updates section-bound constraints.
         */
        void setup_section_bounds_state(const std::shared_ptr<model::DocraftNode> &node,
                                        LayoutComputationState &state);

        /**
         * @brief Configures container-origin cursors for rectangle-like nodes with children.
         */
        void setup_container_cursor_state(const std::shared_ptr<model::DocraftNode> &node,
                                          LayoutComputationState &state);

        /**
         * @brief Pushes layout orientation (horizontal/vertical) on the selected cursor.
         */
        static void setup_layout_direction_state(const std::shared_ptr<model::DocraftNode> &node,
                                                 LayoutComputationState &state);

        std::shared_ptr<DocraftDocumentContext> context_;
        std::vector<std::unique_ptr<generic::DocraftChainOfResponsibilityHandler<model::DocraftNode,
            model::DocraftTransform> > > handlers_;
        handler::DocraftLayoutListHandler *list_handler_ = nullptr;
    };
} // namespace docraft::layout

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
        explicit Impl(std::shared_ptr<DocraftDocumentContext> context, bool reset_cursor);

        const std::shared_ptr<DocraftDocumentContext> &context() const;

        static model::DocraftTransform compute_max_rect(const std::vector<model::DocraftTransform> &boxes);

        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode> &node);

        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode> &node, DocraftCursor &cursor);

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

        void configure_handlers();

        bool compute_node(const std::shared_ptr<model::DocraftNode> &node,
                          model::DocraftTransform *box,
                          DocraftCursor &cursor) const;

        float compute_width(const std::shared_ptr<model::DocraftSection> &node) const;

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

        void advance_to_next_body_page(BodyLayoutState &state);

        bool handle_table_overflow_on_body_page(const std::shared_ptr<model::DocraftNode> &child,
                                                const std::size_t *index_ptr,
                                                const DocraftCursor &child_start_cursor,
                                                BodyLayoutState &state);

        void process_body_child_with_pagination(const std::shared_ptr<model::DocraftNode> &child,
                                                const std::size_t *index_ptr,
                                                BodyLayoutState &state);

        Sections split_sections(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) const;

        SectionPlan build_section_plan(const Sections &sections) const;

        void layout_header_section(const std::shared_ptr<model::DocraftHeader> &header, float header_ratio);

        void layout_body_section(const std::shared_ptr<model::DocraftBody> &body,
                                 const std::shared_ptr<model::DocraftHeader> &header,
                                 const SectionPlan &plan);

        void layout_footer_section(const std::shared_ptr<model::DocraftFooter> &footer,
                                   const std::shared_ptr<model::DocraftBody> &body,
                                   const SectionPlan &plan);

        std::shared_ptr<DocraftDocumentContext> context_;
        std::vector<std::unique_ptr<generic::DocraftChainOfResponsibilityHandler<model::DocraftNode,
            model::DocraftTransform> > > handlers_;
        handler::DocraftLayoutListHandler *list_handler_ = nullptr;
    };
} // namespace docraft::layout


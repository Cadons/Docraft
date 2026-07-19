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

#include "docraft/backend/docraft_backend_providers_factory.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"

namespace docraft::loom::pipeline {
    /**
     * @brief Runs the Measure -> Layout half of the loom pipeline for one region
     * (header/body/footer) of a page, given that region's own content width and cursor
     * origin. Constructed once per DocraftLoomPdfCreator::create() call with the page-wide
     * context (text backend, page width) shared by every region, then invoked once per
     * region with that region's own inputs.
     *
     * Each run() call builds a fresh DocraftLoomMeasureProcessor/DocraftLoomLayoutProcessor
     * pair rather than reusing one pair across regions: a region is a wholly separate
     * traversal, and per-traversal state internal to those processors (e.g.
     * DocraftLoomMeasureProcessor's inherited_wrap_width_, DocraftLoomLayoutProcessor's
     * inherited_width_) must not leak from one region into the next. A fresh instance
     * guarantees that structurally, rather than relying on set_content_width() to know
     * about and reset every such field.
     */
    class DocraftLoomPipelineExecutor
    {
    public:
        /**
         * @brief Constructs the executor with the context shared by every region.
         * @param text_backend Backend used for text measurement.
         * @param page_width Full page width, used to construct each region's
         * DocraftLoomLayoutProcessor (constrains its incoming_width() at the root).
         */
        DocraftLoomPipelineExecutor(std::shared_ptr<backend::IDocraftTextRenderingBackend> text_backend,
                                    float page_width);

        /**
         * @brief Measures then lays out `node`'s subtree, mutating each node's layout_box.
         * @param node Region root (header/footer/body) to measure and lay out.
         * @param content_width Width available to this region's content (page_width minus
         * that region's own margins).
         * @param cursor_x Region content origin's x (after that region's own left margin).
         * @param cursor_y Region content origin's y (after that region's own top margin).
         * @param assign_fixed_page_index Optional. When true, once layout completes, stamps
         * every node in the subtree with page_index -1 (via
         * DocraftLoomPaginationProcessor::assign_page_index_recursive()) if `node` is a
         * nodes::DocraftLoomNode -- for header/footer, which are laid out once and
         * re-rendered on every physical page rather than participating in the body's
         * per-page pagination (DocraftLoomPaginationProcessor::paginate_body()), so the
         * body must leave this false (the default).
         */
        void run(interfaces::DocraftLoomIVisitorNode& node, float content_width, float cursor_x, float cursor_y,
                 bool assign_fixed_page_index = false) const;

    private:
        std::shared_ptr<backend::IDocraftTextRenderingBackend> text_backend_;
        float page_width_;
    };
} // namespace docraft::loom::pipeline
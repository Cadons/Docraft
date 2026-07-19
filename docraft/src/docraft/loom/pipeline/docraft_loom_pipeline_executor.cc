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

#include "docraft/loom/pipeline/docraft_loom_pipeline_executor.h"

#include <utility>

#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"

namespace docraft::loom::pipeline {
    DocraftLoomPipelineExecutor::DocraftLoomPipelineExecutor(
        std::shared_ptr<backend::IDocraftTextRenderingBackend> text_backend, float page_width)
        : text_backend_(std::move(text_backend)), page_width_(page_width)
    {
    }

    void DocraftLoomPipelineExecutor::run(interfaces::DocraftLoomIVisitorNode& node, float content_width,
                                          float cursor_x, float cursor_y, bool assign_fixed_page_index) const
    {
        auto measure = DocraftLoomMeasureProcessor(text_backend_);
        measure.set_content_width(content_width);
        node.accept(measure);

        auto layout = DocraftLoomLayoutProcessor(page_width_);
        layout.set_content_width(content_width);
        layout.reset_cursor(cursor_x, cursor_y);
        node.accept(layout);

        if (assign_fixed_page_index)
        {
            if (auto* concrete_node = dynamic_cast<nodes::DocraftLoomNode*>(&node))
            {
                DocraftLoomPaginationProcessor::assign_page_index_recursive(*concrete_node, -1);
            }
        }
    }
} // namespace docraft::loom::pipeline

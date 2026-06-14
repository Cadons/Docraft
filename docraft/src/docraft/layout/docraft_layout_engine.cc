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

#include "docraft/layout/docraft_layout_engine.h"

#include <memory>
#include <vector>

#include "docraft_layout_engine_impl.h"

namespace docraft::layout {
    DocraftLayoutEngine::DocraftLayoutEngine(const std::shared_ptr<DocraftDocumentContext> &context,
                                             const bool reset_cursor)
        : impl_(std::make_unique<Impl>(context, reset_cursor)) {
    }

    DocraftLayoutEngine::DocraftLayoutEngine(DocraftLayoutEngine &&) noexcept = default;

    DocraftLayoutEngine &DocraftLayoutEngine::operator=(DocraftLayoutEngine &&) noexcept = default;

    DocraftLayoutEngine::~DocraftLayoutEngine() {
    }

    const std::shared_ptr<DocraftDocumentContext> &DocraftLayoutEngine::context() const {
        return impl_->context();
    }

    model::DocraftTransform DocraftLayoutEngine::compute_max_rect(const std::vector<model::DocraftTransform> &boxes) {
        return Impl::compute_max_rect(boxes);
    }

    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node) {
        return impl_->compute_layout(node);
    }

    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                                DocraftCursor &cursor) {
        return impl_->compute_layout(node, cursor);
    }

    void DocraftLayoutEngine::compute_document_layout(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) {
        impl_->compute_document_layout(nodes);
    }
} // namespace docraft::layout

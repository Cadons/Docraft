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

#include "docraft/renderer/painter/docraft_blank_line_painter.h"

namespace docraft::renderer::painter {
    docraft_blank_line_painter::docraft_blank_line_painter(const model::DocraftBlankLine &blank_line_node) : blank_line_node_(
        blank_line_node) {
    }

    void docraft_blank_line_painter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        // Blank line does not render anything
    }
} // docraft

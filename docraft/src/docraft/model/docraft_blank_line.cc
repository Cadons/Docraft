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

#include "docraft/model/docraft_blank_line.h"

#include <iostream>

#include "docraft/renderer/docraft_renderer.h"

namespace docraft::model {
    void DocraftBlankLine::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        // An empty line does not render anything, but we can log its presence if needed
        context->renderer()->render_blank_line(*this);
        // std::cout << "Drawing an empty line at position (" << x() << ", " << y() << ")\n";

    }

    std::shared_ptr<DocraftNode> DocraftBlankLine::clone() const {
        return std::make_shared<DocraftBlankLine>(*this);
    }
} // docraft

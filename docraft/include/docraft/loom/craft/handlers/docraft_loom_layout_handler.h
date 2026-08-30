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

#include "docraft/docraft_lib.h"
#include "docraft/loom/craft/handlers/i_docraft_loom_tag_handler.h"

namespace docraft::loom::craft {
    /**
     * @brief `<Layout orientation="horizontal|vertical">`: builds a `DocraftLoomHStack` or
     * `DocraftLoomVStack` depending on `orientation`, collecting each child's `weight`
     * attribute (validated at parse time to appear only here).
     */
    class DOCRAFT_LIB DocraftLoomLayoutHandler : public IDocraftLoomTagHandler
    {
    public:
        std::shared_ptr<nodes::DocraftLoomNode> build(const docraft::craft::DocraftParsedElement& element,
                                                        DocraftLoomTableHandlerContext& context) override;
    };
} // namespace docraft::loom::craft

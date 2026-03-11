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

#include "docraft/model/docraft_node.h"
#include "docraft/model/i_docraft_clonable.h"

namespace docraft::model {
    std::shared_ptr<DocraftNode> clone_node(const std::shared_ptr<DocraftNode> &node);
    std::vector<std::shared_ptr<DocraftNode>> clone_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes);
} // namespace docraft::model

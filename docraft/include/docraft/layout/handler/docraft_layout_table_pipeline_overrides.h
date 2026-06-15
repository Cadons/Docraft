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

// Shared override declarations used by horizontal/vertical table handlers.
#define DOCRAFT_TABLE_PIPELINE_OVERRIDES \
    void setup_pipeline_state(const std::shared_ptr<model::DocraftTable> &node, \
                              DocraftCursor &cursor) override; \
    void prepare_table_layout(const std::shared_ptr<model::DocraftTable> &node) override; \
    [[nodiscard]] float layout_table_content(const std::shared_ptr<model::DocraftTable> &node) override; \
    [[nodiscard]] float resolve_table_width() const override


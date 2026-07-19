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

#include <vector>

namespace docraft::loom::pipeline {
    /**
     * @brief Divides `available_width` across `count` items by weight -- a missing
     * (short `weights`) or non-positive entry defaults to 1.0, i.e. an even share.
     * Shared by DocraftLoomMeasureProcessor and DocraftLoomLayoutProcessor's HStack and
     * Table weighted-column code, so both stages resolve the same weights the same way.
     * @param available_width Total width to divide among the `count` items.
     * @param weights Per-item weight; a missing or non-positive entry defaults to 1.0.
     * @param count Number of items to resolve a share for.
     * @param floors Optional per-item floor (e.g. each item's own natural/measured
     * width); if non-empty (same size as `count`), no result is smaller than its floor.
     * @return `count` resolved widths.
     */
    std::vector<float> distribute_weighted_widths(float available_width,
                                                  const std::vector<float>& weights,
                                                  int count,
                                                  const std::vector<float>& floors = {});
} // namespace docraft::loom::pipeline
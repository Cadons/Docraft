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
     * @brief Divides `available_amount` across `count` items by weight -- a missing
     * (short `weights`) or non-positive entry defaults to 1.0, i.e. an even share.
     * Axis-agnostic: shared by DocraftLoomMeasureProcessor and
     * DocraftLoomLayoutProcessor's HStack/Table weighted-width code and
     * DocraftLoomLayoutProcessor's VStack weighted-height code, so all resolve the same
     * weights the same way regardless of which axis they're dividing.
     * @param available_amount Total amount (width or height) to divide among the
     * `count` items.
     * @param weights Per-item weight; a missing or non-positive entry defaults to 1.0.
     * @param count Number of items to resolve a share for.
     * @param floors Optional per-item floor (e.g. each item's own natural/measured
     * size on that axis); if non-empty (same size as `count`), no result is smaller
     * than its floor.
     * @return `count` resolved shares.
     */
    std::vector<float> distribute_weighted_amounts(float available_amount,
                                                  const std::vector<float>& weights,
                                                  int count,
                                                  const std::vector<float>& floors = {});

    /**
     * @brief Inputs for resolve_fixed_and_flexible_amounts(). Grouped into a struct
     * instead of positional parameters because weights/explicit_amounts/floors are
     * three same-typed `vector<float>` in a row -- an easy place to pass the wrong one
     * to the wrong slot without the compiler catching it.
     */
    struct WeightedSplitRequest
    {
        /// Total amount (width or height) to divide among `count` items.
        float available_amount = 0.0F;
        /// Number of items to resolve a share for.
        int count = 0;
        /// Per-item weight, applied only to flexible items; a missing or non-positive
        /// entry defaults to 1.0.
        std::vector<float> weights;
        /// Per-item explicit override; a missing or non-positive entry means that item
        /// is flexible. Empty (the default) means every item is flexible.
        std::vector<float> explicit_amounts;
        /// Optional per-flexible-item floor (e.g. each item's own natural/measured
        /// size on that axis); if non-empty (same size as `count`), no flexible item's
        /// result is smaller than its floor.
        std::vector<float> floors;
    };

    /**
     * @brief Like distribute_weighted_amounts, but items can opt out of the weighted
     * split entirely: an item with a positive `explicit_amounts[i]` is "fixed" -- it
     * keeps that exact value, doesn't count toward the weight pool, and its amount is
     * subtracted from `available_amount` first. Only what's left over is then split by
     * weight (floored per `floors`) among the remaining, flexible items -- via a plain
     * distribute_weighted_amounts() call restricted to that flexible subset, so both
     * functions apply the exact same weight-default and floor rules.
     *
     * Both table-column-width call sites need this same two-tier split: a column with
     * an explicit Cell width="..." on any row must resolve to exactly that width, and
     * only the other columns compete for whatever space is left.
     * DocraftLoomLayoutProcessor::resolve_table_column_widths calls this once each
     * cell's natural width is known (passed as `floors`); DocraftLoomMeasureProcessor
     * calls it earlier, to estimate a wrap budget before any cell has been measured
     * (so with no `floors`). Sharing this function is what keeps the two in sync --
     * without it, Measure's estimate can silently drift from what Layout later paints.
     * @param request See WeightedSplitRequest.
     * @return `request.count` resolved shares, summing to `request.available_amount`
     * unless the floors force a flexible item above its weighted share.
     */
    std::vector<float> resolve_fixed_and_flexible_amounts(const WeightedSplitRequest& request);
} // namespace docraft::loom::pipeline
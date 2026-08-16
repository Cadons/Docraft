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

#include "docraft/loom/pipeline/docraft_loom_weighted_distribution.h"

#include <algorithm>

namespace docraft::loom::pipeline {
    std::vector<float> distribute_weighted_amounts(float available_amount,
                                                  const std::vector<float>& weights,
                                                  int count,
                                                  const std::vector<float>& floors)
    {
        std::vector<float> resolved(static_cast<std::size_t>(std::max(count, 0)), 0.0F);
        if (count <= 0)
        {
            return resolved;
        }

        float total_weight = 0.0F;
        std::vector<float> effective_weights(static_cast<std::size_t>(count));
        for (int i = 0; i < count; ++i)
        {
            const float w = i < static_cast<int>(weights.size()) && weights[static_cast<std::size_t>(i)] > 0.0F
                                ? weights[static_cast<std::size_t>(i)]
                                : 1.0F;
            effective_weights[static_cast<std::size_t>(i)] = w;
            total_weight += w;
        }

        for (int i = 0; i < count; ++i)
        {
            const float share = available_amount * effective_weights[static_cast<std::size_t>(i)] / total_weight;
            resolved[static_cast<std::size_t>(i)] =
                floors.empty() ? share : std::max(floors[static_cast<std::size_t>(i)], share);
        }
        return resolved;
    }

    std::vector<float> resolve_fixed_and_flexible_amounts(const WeightedSplitRequest& request)
    {
        const int count = request.count;
        std::vector<float> resolved(static_cast<std::size_t>(std::max(count, 0)), 0.0F);
        if (count <= 0)
        {
            return resolved;
        }

        auto is_fixed = [&](int i) {
            return i < static_cast<int>(request.explicit_amounts.size()) &&
                   request.explicit_amounts[static_cast<std::size_t>(i)] > 0.0F;
        };

        // 1) Fixed items keep their own amount verbatim, and their share is subtracted
        // from `remaining` so it's never up for grabs by the flexible items below.
        // While reserving fixed items, also collect the flexible ones' own weight/floor
        // into their own compacted vectors (indices 0..flexible_count-1), so step 2 can
        // hand them to a plain distribute_weighted_amounts() call.
        float remaining = request.available_amount;
        std::vector<int> flexible_indices;
        std::vector<float> flexible_weights;
        std::vector<float> flexible_floors;
        const bool has_floors = !request.floors.empty();
        for (int i = 0; i < count; ++i)
        {
            if (is_fixed(i))
            {
                resolved[static_cast<std::size_t>(i)] = request.explicit_amounts[static_cast<std::size_t>(i)];
                remaining -= resolved[static_cast<std::size_t>(i)];
                continue;
            }
            flexible_indices.push_back(i);
            flexible_weights.push_back(
                i < static_cast<int>(request.weights.size()) ? request.weights[static_cast<std::size_t>(i)] : 0.0F);
            if (has_floors)
                flexible_floors.push_back(request.floors[static_cast<std::size_t>(i)]);
        }
        remaining = std::max(0.0F, remaining);

        // 2) Split `remaining` among just the flexible items -- reusing
        // distribute_weighted_amounts() here (instead of re-deriving its weight-default
        // and floor logic) is what guarantees a flexible item is treated identically
        // whether it's resolved through this function or a plain weighted split.
        const auto flexible_shares = distribute_weighted_amounts(
            remaining, flexible_weights, static_cast<int>(flexible_indices.size()), flexible_floors);

        float flexible_total = 0.0F;
        for (float share : flexible_shares)
            flexible_total += share;

        // 3) A floor in step 2 can push a flexible item above its weighted share, so the
        // flexible items might no longer add up to `remaining`. Scale just those items
        // (fixed ones stay untouched) so the total matches `available_amount` whenever
        // the floors allow it.
        const float scale = (flexible_total > 0.0F && remaining > 0.0F) ? remaining / flexible_total : 1.0F;
        for (std::size_t k = 0; k < flexible_indices.size(); ++k)
            resolved[static_cast<std::size_t>(flexible_indices[k])] = flexible_shares[k] * scale;

        return resolved;
    }
} // namespace docraft::loom::pipeline
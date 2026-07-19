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
    std::vector<float> distribute_weighted_widths(float available_width,
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
            const float share = available_width * effective_weights[static_cast<std::size_t>(i)] / total_weight;
            resolved[static_cast<std::size_t>(i)] =
                floors.empty() ? share : std::max(floors[static_cast<std::size_t>(i)], share);
        }
        return resolved;
    }
} // namespace docraft::loom::pipeline
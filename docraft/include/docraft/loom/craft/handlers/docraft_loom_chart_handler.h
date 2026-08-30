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

#include <string>

#include "docraft/docraft_lib.h"
#include "docraft/loom/craft/handlers/i_docraft_loom_tag_handler.h"

namespace docraft::loom::craft {
    class DOCRAFT_LIB DocraftLoomChartHandler : public IDocraftLoomTagHandler
    {
    public:
        std::shared_ptr<nodes::DocraftLoomNode> build(const docraft::craft::DocraftParsedElement& element,
                                                        DocraftLoomTableHandlerContext& context) override;

    private:
        /**
         * @brief Result of resolve_series_points(): `points` and `labels` are parallel
         * arrays (same size, same index).
         * @details See `charts::DocraftChartSeries::point_labels` for how the label side
         * is consumed downstream.
         */
        struct ResolvedSeriesPoints;

        /**
         * @brief Resolves a `<Series model="...">` attribute into numeric data points.
         * @details Applies `${...}` substitution, single-quote-JSON normalization, then
         * JSON-parses the result. Each array entry must be one of: a 2-element numeric
         * array (`[x,y]`); an object with numeric `x`/`y` keys; or a single-key object
         * (`{"label": value}`, the shape pie/histogram model data always uses) -- which
         * becomes `x` = the entry's ordinal index (0-based), `y` = the value, and `label`
         * = the key.
         * @throws docraft::exception::DataFormatException if `raw` doesn't resolve to a
         * JSON array, or any entry doesn't match one of the three accepted shapes.
         */
        static ResolvedSeriesPoints resolve_series_points(const std::string& raw, DocraftLoomTableHandlerContext& context);
    };
} // namespace docraft::loom::craft

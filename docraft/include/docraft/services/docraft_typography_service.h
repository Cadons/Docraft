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
#include "docraft/generic/docraft_font_applier.h"
#include <memory>

namespace docraft::services {
    /**
     * @brief Manages typography and font handling.
     *
     * Responsible for:
     * - Holding the font applier instance
     * - Applying font to text nodes
     */
    class DOCRAFT_LIB TypographyService {
    public:
        TypographyService();

        ~TypographyService();

        /**
         * @brief Returns the font applier instance.
         */
        [[nodiscard]] std::shared_ptr<const generic::DocraftFontApplier> font_applier() const;

        [[nodiscard]] std::shared_ptr<generic::DocraftFontApplier> edit_font_applier();

        /**
         * @brief Sets the font applier.
         */
        void set_font_applier(const std::shared_ptr<generic::DocraftFontApplier> &applier);

    private:
        std::shared_ptr<generic::DocraftFontApplier> font_applier_;
    };
} // namespace docraft::services



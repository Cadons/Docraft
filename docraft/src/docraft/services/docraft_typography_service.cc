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

#include "docraft/services/docraft_typography_service.h"
#include "docraft/generic/docraft_font_applier.h"

namespace docraft::services {
    TypographyService::TypographyService() = default;

    TypographyService::~TypographyService() = default;

    std::shared_ptr<const generic::DocraftFontApplier> TypographyService::font_applier() const {
        return font_applier_;
    }

    std::shared_ptr<generic::DocraftFontApplier> TypographyService::edit_font_applier() {
        return font_applier_;
    }

    void TypographyService::set_font_applier(const std::shared_ptr<generic::DocraftFontApplier> &applier) {
        font_applier_ = applier;
    }
} // namespace docraft::services

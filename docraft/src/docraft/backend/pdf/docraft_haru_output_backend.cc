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

#include "docraft/backend/pdf/docraft_haru_output_backend.h"

#include <hpdf.h>

#include "docraft/exception/docraft_exceptions.h"

namespace docraft::backend::pdf {
    DocraftHaruOutputBackend::DocraftHaruOutputBackend(const std::shared_ptr<DocraftHaruSharedState> &state)
        : state_(state) {
    }

    void DocraftHaruOutputBackend::save_to_file(const std::string &path) const {
        const auto pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            throw docraft::exception::BackendStateException("Haru document is not initialized");
        }
        HPDF_SaveToFile(pdf, path.c_str());
    }

    std::string DocraftHaruOutputBackend::file_extension() const {
        return ".pdf";
    }
} // namespace docraft::backend::pdf

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

namespace docraft::backend::pdf {
    DocraftHaruBackend::OutputHaruBackend::OutputHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {
    }

    void DocraftHaruBackend::OutputHaruBackend::save_to_file(const std::string &path) const {
        HPDF_SaveToFile(owner_.pdf_, path.c_str());
    }

    std::string DocraftHaruBackend::OutputHaruBackend::file_extension() const {
        return ".pdf";
    }
} // namespace docraft::backend::pdf

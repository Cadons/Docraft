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

#include "docraft/backend/pdf/docraft_haru_backend.h"

namespace docraft::backend::pdf {
    /**
     * @brief Haru implementation of metadata operations.
     */
    class DocraftHaruBackend::MetadataHaruBackend : public docraft::backend::IDocraftMetadataBackend {
    public:
        explicit MetadataHaruBackend(DocraftHaruBackend &owner);

        void set_document_metadata(const DocraftDocumentMetadata &metadata) override;

    private:
        DocraftHaruBackend &owner_;
    };
} // namespace docraft::backend::pdf


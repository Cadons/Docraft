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

#include <string>

namespace docraft::backend {
    /**
     * @brief Capability interface for document output operations.
     */
    class DOCRAFT_LIB IDocraftOutputBackend {
    public:
        virtual ~IDocraftOutputBackend() = default;

        /**
         * @brief Saves the document to a file path.
         * @param path Output file path.
         */
        virtual void save_to_file(const std::string &path) const = 0;

        /**
         * @brief Returns the preferred file extension for this backend.
         * @return Extension with or without leading dot (e.g. ".pdf" or "pdf").
         */
        [[nodiscard]] virtual std::string file_extension() const = 0;
    };
} // namespace docraft::backend


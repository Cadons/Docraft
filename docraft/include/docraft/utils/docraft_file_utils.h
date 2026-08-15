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

#include <cstddef>
#include <filesystem>
#include <optional>

namespace docraft::utils {
    /**
     * @brief Filesystem helpers shared across backends.
     */
    class DOCRAFT_LIB DocraftFileUtils
    {
    public:
        DocraftFileUtils() = delete;

        /**
         * @brief Securely writes raw bytes to a new, uniquely-named temporary file.
         *
         * The file is created inside a private, owner-only subdirectory of the
         * system temp root rather than directly in that root -- the root itself
         * (e.g. /tmp, %TEMP%) is shared with every other local user, so a file
         * placed straight into it can be read or raced by them while it exists.
         * std::filesystem::create_directory() only succeeds if the name didn't
         * already exist, which is an exclusive-create against a guessed/pre-planted
         * path (CWE-377/CWE-59) on any platform, so no platform-specific temp-file
         * API (mkstemp, _mktemp_s, ...) is needed; the directory is then restricted
         * to owner-only before the file is written into it.
         *
         * @param data Raw bytes to write. Must not be null when size > 0.
         * @param size Number of bytes to write.
         * @return Path to the created file, or std::nullopt on failure. On failure no
         * partially-written file or subdirectory is left behind.
         */
        static std::optional<std::filesystem::path> write_temp_file(const unsigned char* data, std::size_t size);

        /**
         * @brief Removes a file, ignoring errors (e.g. already removed).
         *
         * If path was produced by write_temp_file(), also removes the now-empty
         * private subdirectory it was created in.
         *
         * @param path File to remove.
         */
        static void remove_file(const std::filesystem::path& path);
    };
}

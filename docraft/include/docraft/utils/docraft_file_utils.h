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
         * Uses the platform's atomic exclusive-create primitive (POSIX mkstemp /
         * Windows O_CREAT|O_EXCL) rather than picking a name ourselves and opening it
         * in a second step -- that atomicity is what stops a local attacker from
         * pre-planting a symlink at a guessed path and redirecting the write elsewhere
         * (CWE-377/CWE-59). The name itself doesn't need to be unpredictable: exclusive
         * creation fails safely regardless of how guessable it is, so letting the OS
         * pick one it knows to be unused is simpler and no less safe than hand-rolling
         * one with a PRNG.
         *
         * @param data Raw bytes to write. Must not be null when size > 0.
         * @param size Number of bytes to write.
         * @return Path to the created file, or std::nullopt on failure. On failure no
         * partially-written file is left behind.
         */
        static std::optional<std::filesystem::path> write_temp_file(const unsigned char* data, std::size_t size);

        /**
         * @brief Removes a file, ignoring errors (e.g. already removed).
         * @param path File to remove.
         */
        static void remove_file(const std::filesystem::path& path);
    };
}

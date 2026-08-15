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

#include "docraft/utils/docraft_file_utils.h"

#include <format>
#include <fstream>
#include <random>
#include <string>

namespace {
    // Name of the file created inside each private per-call subdirectory; the
    // subdirectory itself is what guarantees uniqueness, so a fixed name is fine.
    constexpr auto kTempFileName = "docraft.tmp";
    constexpr auto kTempDirPrefix = "docraft_";
    constexpr int kMaxDirCreateAttempts = 8;
}

namespace docraft::utils {
    std::optional<std::filesystem::path> DocraftFileUtils::write_temp_file(const unsigned char* data,
                                                                             std::size_t size)
    {
        if (!data || size == 0)
        {
            return std::nullopt;
        }

        std::error_code ec;
        const auto tmp_root = std::filesystem::temp_directory_path(ec);//is safe
        if (ec)
        {
            return std::nullopt;
        }

        // The system temp root (e.g. /tmp, %TEMP%) is shared with every other
        // local user, so writing into it directly lets them read or race the
        // file while it exists. Carve out our own subdirectory first:
        // create_directory() only succeeds if the name didn't already exist,
        // which is an exclusive-create against a guessed/pre-planted path
        // (CWE-377/CWE-59) on any platform without needing a platform-specific
        // primitive (mkstemp, _mktemp_s, ...); restricting it to owner-only
        // right after closes it off to other local users too.
        std::mt19937_64 rng(std::random_device{}());
        std::filesystem::path private_dir;
        for (int attempt = 0; attempt < kMaxDirCreateAttempts; ++attempt)
        {
            auto candidate = tmp_root / (std::format("{}{:016x}", kTempDirPrefix, rng()));
            if (std::filesystem::create_directory(candidate, ec))
            {
                private_dir = std::move(candidate);
                break;
            }
        }
        if (private_dir.empty())
        {
            return std::nullopt;
        }

#if !defined(_WIN32)
        // POSIX temp roots (e.g. /tmp) are shared by every local user, so lock the
        // subdirectory down to owner-only. Skip this on Windows: %TEMP% already
        // resolves to a per-user, ACL-isolated directory there, and MSVC STL's
        // permissions() has been observed to leave the directory unwritable even
        // to its owner afterwards, which broke file creation below in CI.
        std::filesystem::permissions(private_dir, std::filesystem::perms::owner_all,
                                      std::filesystem::perm_options::replace, ec);
#endif

        const auto tmp_path = private_dir / kTempFileName;
        std::ofstream out(tmp_path, std::ios::binary | std::ios::trunc);
        if (out)
        {
            out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
        }
        const bool ok = out.good();
        out.close();
        if (!ok)
        {
            remove_file(tmp_path);
            std::filesystem::remove(private_dir, ec);
            return std::nullopt;
        }

        return tmp_path;
    }

    void DocraftFileUtils::remove_file(const std::filesystem::path& path)
    {
        std::error_code ec;
        std::filesystem::remove(path, ec);

        // Also clean up the private per-call subdirectory write_temp_file creates
        // around its file. Only attempt this for paths that actually look like
        // one of ours (right filename, parent dir named with our prefix) -- this
        // is a general-purpose file removal helper, so it must never reach for an
        // arbitrary caller's parent directory, let alone the shared system temp
        // root itself.
        const auto& parent = path.parent_path();
        if (path.filename() == kTempFileName && parent.filename().string().starts_with(kTempDirPrefix))
        {
            std::filesystem::remove(parent, ec);
        }
    }
}

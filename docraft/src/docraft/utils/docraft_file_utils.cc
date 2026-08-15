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

#include <vector>

#if defined(_WIN32)
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <sys/stat.h>
#else
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace {
    // Name of the file created inside each private per-call subdirectory; the
    // subdirectory itself is what guarantees uniqueness, so a fixed name is fine.
    constexpr auto kTempFileName = "docraft.tmp";
    constexpr auto kTempDirPrefix = "docraft_";
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
        const auto tmp_root = std::filesystem::temp_directory_path(ec);
        if (ec)
        {
            return std::nullopt;
        }

        // The system temp root (e.g. /tmp, %TEMP%) is shared with every other
        // local user, so writing into it directly lets them read or race the
        // file while it exists. Carve out a private, current-user-only (0700)
        // subdirectory first and write into that instead.
        auto dir_tmpl_str = (tmp_root / "docraft_XXXXXX").string();
        std::vector<char> dir_tmpl(dir_tmpl_str.begin(), dir_tmpl_str.end());
        dir_tmpl.push_back('\0');

#if defined(_WIN32)
        if (_mktemp_s(dir_tmpl.data(), dir_tmpl.size()) != 0 || _mkdir(dir_tmpl.data()) != 0)
        {
            return std::nullopt;
        }
        const std::filesystem::path private_dir(dir_tmpl.data());
        const std::filesystem::path tmp_path = private_dir / kTempFileName;

        int fd = -1;
        if (_sopen_s(&fd, tmp_path.string().c_str(), _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                     _SH_DENYRW, _S_IWRITE) != 0 || fd == -1)
        {
            std::filesystem::remove(private_dir, ec);
            return std::nullopt;
        }
        const auto written = _write(fd, data, static_cast<unsigned int>(size));
        _close(fd);
        if (written < 0 || static_cast<std::size_t>(written) != size)
        {
            remove_file(tmp_path);
            std::filesystem::remove(private_dir, ec);
            return std::nullopt;
        }
#else
        if (mkdtemp(dir_tmpl.data()) == nullptr)
        {
            return std::nullopt;
        }
        const std::filesystem::path private_dir(dir_tmpl.data());
        const std::filesystem::path tmp_path = private_dir / kTempFileName;

        const int fd = open(tmp_path.c_str(), O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);
        if (fd == -1)
        {
            std::filesystem::remove(private_dir, ec);
            return std::nullopt;
        }
        std::size_t total_written = 0;
        while (total_written < size)
        {
            const ssize_t n = write(fd, data + total_written, size - total_written);
            if (n <= 0)
            {
                break;
            }
            total_written += static_cast<std::size_t>(n);
        }
        close(fd);
        if (total_written != size)
        {
            remove_file(tmp_path);
            std::filesystem::remove(private_dir, ec);
            return std::nullopt;
        }
#endif

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

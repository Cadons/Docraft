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
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <sys/stat.h>
#else
#include <cstdlib>
#include <unistd.h>
#endif

namespace docraft::utils {
    std::optional<std::filesystem::path> DocraftFileUtils::write_temp_file(const unsigned char* data,
                                                                             std::size_t size)
    {
        if (!data || size == 0)
        {
            return std::nullopt;
        }

        std::error_code ec;
        const auto tmp_dir = std::filesystem::temp_directory_path(ec);
        if (ec)
        {
            return std::nullopt;
        }

        auto tmpl_str = (tmp_dir / "docraft_XXXXXX").string();
        std::vector<char> tmpl(tmpl_str.begin(), tmpl_str.end());
        tmpl.push_back('\0');

#if defined(_WIN32)
        if (_mktemp_s(tmpl.data(), tmpl.size()) != 0)
        {
            return std::nullopt;
        }
        const std::filesystem::path tmp_path(tmpl.data());
        int fd = -1;
        if (_sopen_s(&fd, tmp_path.string().c_str(), _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                     _SH_DENYRW, _S_IWRITE) != 0 || fd == -1)
        {
            return std::nullopt;
        }
        const auto written = _write(fd, data, static_cast<unsigned int>(size));
        _close(fd);
        if (written < 0 || static_cast<std::size_t>(written) != size)
        {
            remove_file(tmp_path);
            return std::nullopt;
        }
#else
        const int fd = mkstemp(tmpl.data());
        if (fd == -1)
        {
            return std::nullopt;
        }
        const std::filesystem::path tmp_path(tmpl.data());
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
            return std::nullopt;
        }
#endif

        return tmp_path;
    }

    void DocraftFileUtils::remove_file(const std::filesystem::path& path)
    {
        std::error_code ec;
        std::filesystem::remove(path, ec);
    }
}

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

#include "docraft/backend/pdf/docraft_haru_font_backend.h"

#include <cstdio>
#include <filesystem>
#include <stdexcept>
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

#include <hpdf.h>

namespace docraft::backend::pdf {
    using exception::BackendStateException;

    DocraftHaruFontBackend::DocraftHaruFontBackend(const std::shared_ptr<DocraftHaruSharedState> &state)
        : state_(state) {
    }

    const char *DocraftHaruFontBackend::register_ttf_font_from_file(const std::string &path,
                                                                    bool embed) const {
        auto *const pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            return nullptr;
        }
        const char *result = HPDF_LoadTTFontFromFile(pdf,
                                                     path.c_str(),
                                                     embed ? HPDF_TRUE : HPDF_FALSE);
        if (!result) {
            HPDF_ResetError(pdf);
        }
        return result;
    }

    const char *DocraftHaruFontBackend::register_ttf_font_from_memory(const unsigned char *data,
                                                                       std::size_t size,
                                                                       bool embed) const {
        auto *const pdf = state_ ? state_->pdf : nullptr;
        if (!pdf || !data || size == 0) {
            return nullptr;
        }

        // HPDF_LoadTTFontFromMemory doesn't exist in the libharu version vcpkg
        // distributes (only Homebrew's is new enough) -- write to a temp file and go
        // through HPDF_LoadTTFontFromFile instead, so this works against any libharu
        // version. libharu copies the font bytes into its own internal structures at
        // load time, so the temp file can be removed right after the call.
        //
        // The temp directory is world-writable (CWE-377), so the file itself must be
        // created with an atomic exclusive-create primitive that refuses to follow a
        // pre-existing file or symlink at the target path (CWE-59) -- POSIX mkstemp /
        // Windows O_CREAT|O_EXCL. That atomicity is what actually closes the attack, so
        // deliberately not hand-rolling the name with a PRNG (CWE-338): exclusive
        // creation fails safely no matter how guessable the name is, so letting the OS
        // pick a name it knows to be unused is strictly simpler and no less safe.
        std::error_code ec;
        const auto tmp_dir = std::filesystem::temp_directory_path(ec);
        if (ec) {
            return nullptr;
        }

        auto tmpl_str = (tmp_dir / "docraft_font_XXXXXX").string();
        std::vector<char> tmpl(tmpl_str.begin(), tmpl_str.end());
        tmpl.push_back('\0');

#if defined(_WIN32)
        if (_mktemp_s(tmpl.data(), tmpl.size()) != 0) {
            return nullptr;
        }
        const std::filesystem::path tmp_path(tmpl.data());
        int fd = -1;
        if (_sopen_s(&fd, tmp_path.string().c_str(), _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                     _SH_DENYRW, _S_IWRITE) != 0 || fd == -1) {
            return nullptr;
        }
        const auto written = _write(fd, data, static_cast<unsigned int>(size));
        _close(fd);
        if (written < 0 || static_cast<std::size_t>(written) != size) {
            std::filesystem::remove(tmp_path, ec);
            return nullptr;
        }
#else
        const int fd = mkstemp(tmpl.data());
        if (fd == -1) {
            return nullptr;
        }
        const std::filesystem::path tmp_path(tmpl.data());
        std::size_t total_written = 0;
        while (total_written < size) {
            const ssize_t n = write(fd, data + total_written, size - total_written);
            if (n <= 0) {
                break;
            }
            total_written += static_cast<std::size_t>(n);
        }
        close(fd);
        if (total_written != size) {
            std::filesystem::remove(tmp_path, ec);
            return nullptr;
        }
#endif

        const char *result = HPDF_LoadTTFontFromFile(pdf,
                                                      tmp_path.string().c_str(),
                                                      embed ? HPDF_TRUE : HPDF_FALSE);
        std::filesystem::remove(tmp_path, ec);
        if (!result) {
            HPDF_ResetError(pdf);
        }
        return result;
    }

    bool DocraftHaruFontBackend::can_use_font(const std::string &internal_name,
                                              const char *encoder) const {
        auto *const pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            return false;
        }
        HPDF_Font font = HPDF_GetFont(pdf, internal_name.c_str(), encoder);
        if (!font || HPDF_GetError(pdf) != HPDF_OK) {
            HPDF_ResetError(pdf);
            return false;
        }
        return true;
    }

    void DocraftHaruFontBackend::set_font(const std::string &internal_name,
                                          float size,
                                          const char *encoder) const {
        auto *const pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            throw BackendStateException("Haru document is not initialized");
        }
        HPDF_Font font = HPDF_GetFont(pdf, internal_name.c_str(), encoder);
        if (!font) {
            throw BackendStateException("Failed to resolve font: " + internal_name);
        }
        const auto *provider = state_->ensure_page_provider();
        HPDF_Page_SetFontAndSize(provider->current_page(), font, size);
    }
} // namespace docraft::backend::pdf

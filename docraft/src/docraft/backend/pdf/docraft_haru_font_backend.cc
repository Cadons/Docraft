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
#include <random>
#include <stdexcept>

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
        // The temp directory is world-writable, so a predictable name (e.g. an
        // incrementing counter) would let a local attacker pre-plant a symlink at the
        // path we're about to open, redirecting our write to an arbitrary file the
        // process can write to (CWE-377/CWE-59). Guard against that with an
        // unpredictable name plus fopen's "x" mode, which atomically fails instead of
        // following an existing symlink/file rather than opening it.
        std::error_code ec;
        const auto tmp_dir = std::filesystem::temp_directory_path(ec);
        if (ec) {
            return nullptr;
        }

        std::random_device rd;
        std::mt19937_64 rng(rd());
        std::filesystem::path tmp_path;
        std::FILE *file = nullptr;
        for (int attempt = 0; attempt < 8 && !file; ++attempt) {
            char name[64];
            std::snprintf(name, sizeof(name), "docraft_font_%016llx.ttf",
                          static_cast<unsigned long long>(rng()));
            tmp_path = tmp_dir / name;
            file = std::fopen(tmp_path.string().c_str(), "wxb");
        }
        if (!file) {
            return nullptr;
        }

        const std::size_t written = std::fwrite(data, 1, size, file);
        std::fclose(file);
        if (written != size) {
            std::filesystem::remove(tmp_path, ec);
            return nullptr;
        }

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

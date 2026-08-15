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
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace docraft::utils {
    /**
     * @brief Raw font data container.
     */
    struct DocraftFontData
    {
        const unsigned char* data;
        size_t size;
    };

    /**
     * @brief Singleton registry for in-memory and file-based fonts.
     *
     * Stores raw font data so backends can register fonts without re-reading files.
     */
    class DOCRAFT_LIB DocraftFontRegistry
    {
    public:
        /**
         * @brief Returns the singleton instance.
         * @return Reference to the registry singleton.
         */
        static DocraftFontRegistry& instance();

        /**
         * @brief Registers a font from memory.
         * @param name Font family or variant name.
         * @param data Raw font data.
         * @param size Size of the data in bytes.
         * @return true if the font was registered.
         */
        bool register_font(const std::string& name, const unsigned char* data, size_t size);
        /**
         * @brief Registers a font by loading it from a file path.
         * @param name Font family or variant name.
         * @param file_path Path to the font file.
         * @return true on success, false on failure.
         */
        bool register_font(const std::string& name, const std::string& file_path);

        /**
         * @brief Returns font data for a registered name, or nullptr if missing.
         * @param name Font family or variant name.
         * @return Pointer to font data, or nullptr if not found.
         */
        const DocraftFontData* find_font(const std::string& name) const;

        /**
         * @brief Returns the list of registered font names, including aliases (see
         * register_font_alias()) -- DocraftFontResolver builds its family/style index
         * from this list, so an alias must appear here to be resolvable by family+style.
         * @return Vector of font names.
         */
        std::vector<std::string> registered_font_names() const;

        /**
         * @brief Returns the names of fonts registered from raw bytes (e.g. bundled fonts
         * embedded at build time), excluding alias-only names -- unlike
         * registered_font_names(), this does not include entries from register_font_alias().
         * @return Vector of raw font names.
         */
        std::vector<std::string> raw_font_names() const;

        /**
         * @brief Registers an alias for a font name, e.g. mapping a craft-language family
         * name like "OpenSans-Bold" to whatever internal name the backend actually loaded
         * the font under (which the backend chooses, not the caller -- see
         * IDocraftFontBackend::register_ttf_font_from_file()).
         * @param alias Name callers will request (e.g. via `font_name` attributes).
         * @param target_name Backend-internal name to resolve `alias` to.
         */
        void register_font_alias(const std::string& alias, const std::string& target_name);

        /**
         * @brief Resolves a font name through the alias table.
         * @param name Requested name.
         * @return The aliased target name if `name` is a registered alias, otherwise
         * `name` itself unchanged.
         */
        std::string resolve_font_alias(const std::string& name) const;

    private:
        std::unordered_map<std::string, DocraftFontData> registry_;
        std::unordered_map<std::string, std::string> aliases_;
        DocraftFontRegistry() = default;
    };
}

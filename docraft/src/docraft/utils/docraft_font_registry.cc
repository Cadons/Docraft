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

#include "docraft/utils/docraft_font_registry.h"

#include <fstream>
#include <iostream>
#include <memory>

// Defined in the generated fonts.h, compiled into its own translation unit
// (fonts_registration.cc) so the file-scope statics that call register_font() for each
// bundled font actually run. That translation unit exports no other referenced symbol, so
// a static library linker drops it unless something calls this anchor function to force it
// into the final binary.
extern void docraft_register_fonts();

namespace docraft::utils {
    DocraftFontRegistry& DocraftFontRegistry::instance() {
        docraft_register_fonts();
        static DocraftFontRegistry inst;
        return inst;
    }

    bool DocraftFontRegistry::register_font(const std::string& name,
                                            const unsigned char* data,
                                            size_t size) {
        registry_.insert({name, {data, size}});
        return true;
    }

    bool DocraftFontRegistry::register_font(const std::string& name, const std::string& file_path) {
        if (!std::filesystem::exists(file_path)) {
            std::cerr << "Font file " << file_path << " does not exist, skipping." << std::endl;
            return false;
        }
        try {
            auto data = std::make_unique<unsigned char[]>(std::filesystem::file_size(file_path));
            if (std::ifstream file(file_path, std::ios::binary);
                !file.read(reinterpret_cast<char*>(data.get()), std::filesystem::file_size(file_path))) {
                std::cerr << "Failed to read font file: " << file_path << std::endl;
                return false;
            }
            registry_.insert({name, {data.release(), static_cast<size_t>(std::filesystem::file_size(file_path))}});
            return true;
        } catch (...) {
            std::cerr << "Error registering font from file." << std::endl;
            return false;
        }
    }

    const DocraftFontData *DocraftFontRegistry::find_font(const std::string &name) const {
        auto it = registry_.find(name);
        if (it != registry_.end()) {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<std::string> DocraftFontRegistry::registered_font_names() const {
        std::vector<std::string> names;
        names.reserve(registry_.size() + aliases_.size());
        for (const auto &pair : registry_) {
            names.push_back(pair.first);
        }
        for (const auto& pair : aliases_)
        {
            names.push_back(pair.first);
        }
        return names;
    }

    std::vector<std::string> DocraftFontRegistry::raw_font_names() const {
        std::vector<std::string> names;
        names.reserve(registry_.size());
        for (const auto &pair : registry_) {
            names.push_back(pair.first);
        }
        return names;
    }

    void DocraftFontRegistry::register_font_alias(const std::string& alias, const std::string& target_name)
    {
        aliases_[alias] = target_name;
    }

    std::string DocraftFontRegistry::resolve_font_alias(const std::string& name) const
    {
        const auto it = aliases_.find(name);
        return it != aliases_.end() ? it->second : name;
    }
} // namespace docraft::utils

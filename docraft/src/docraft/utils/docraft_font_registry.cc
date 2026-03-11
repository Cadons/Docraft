#include "docraft/utils/docraft_font_registry.h"

#include <fstream>
#include <iostream>
#include <memory>

namespace docraft::utils {
    DocraftFontRegistry& DocraftFontRegistry::instance() {
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
        } catch (const std::exception &e) {
            std::cerr << "Error registering font from file: " << e.what() << std::endl;
            return false;
        }
    }

    const DocraftFontData* DocraftFontRegistry::get_font(const std::string& name) const {
        auto it = registry_.find(name);
        if (it != registry_.end()) {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<std::string> DocraftFontRegistry::registered_font_names() const {
        std::vector<std::string> names;
        names.reserve(registry_.size());
        for (const auto &pair : registry_) {
            names.push_back(pair.first);
        }
        return names;
    }
} // namespace docraft::utils

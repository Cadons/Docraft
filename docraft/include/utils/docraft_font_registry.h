#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace docraft::utils {
    struct DocraftFontData {
        const unsigned char* data;
        size_t size;
    };

    class DocraftFontRegistry {
    public:
        static DocraftFontRegistry& instance() {
            static DocraftFontRegistry inst;
            return inst;
        }

        bool register_font(const std::string& name, const unsigned char* data, size_t size) {
            registry_.insert({name, {data, size}});
            return true;
        }

        const DocraftFontData* get_font(const std::string& name) const {
            auto it = registry_.find(name);
            if (it != registry_.end()) return &it->second;
            return nullptr;
        }
        std::vector<std::string> registered_font_names() const {
            std::vector<std::string> names;
            names.reserve(registry_.size());
            for (const auto &pair : registry_) {
                names.push_back(pair.first);
            }
            return names;
        }

    private:
        std::unordered_map<std::string, DocraftFontData> registry_;
        DocraftFontRegistry() = default;
    };
}

#pragma once
#include <atomic>
#include <filesystem>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
namespace docraft {
    class DocraftDocumentContext;
    namespace model {
        class DocraftText;
    }
}
namespace docraft::generic {
    class DocraftFontApplier {
    public:
        void apply_font(const std::shared_ptr<model::DocraftText>& node) const;

        static std::list<std::string> default_fonts();

        ~DocraftFontApplier();
        static const char* get_font_registred_name(const std::string& name);
        explicit DocraftFontApplier(const std::shared_ptr<DocraftDocumentContext>& context);
        static void set_font_encoding(const std::string& font_name, bool utf8);
        [[nodiscard]] bool is_font_utf8_encoding(const std::string& font_name) const;
    private:
        bool is_font_supported(const std::string &name, const char *encoder) const;
        const char *load_font_data(const std::string &name) const;
        static const inline std::filesystem::path kTempDir = std::filesystem::temp_directory_path();
        static inline std::unordered_map<std::string, std::string> fonts_;
        static inline std::unordered_map<std::string, bool> font_utf8_encoding_;
        static inline std::atomic<int> instance_count_{0};
        std::shared_ptr<DocraftDocumentContext> context_;


    };
}

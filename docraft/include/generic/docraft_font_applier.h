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
    /**
     * @brief Applies fonts to text nodes and manages font registration.
     *
     * Uses the document context backend to register fonts and select
     * appropriate encodings for text rendering.
     */
    class DocraftFontApplier {
    public:
        /**
         * @brief Applies font selection and encoding to a text node.
         * @param node Text node.
         */
        void apply_font(const std::shared_ptr<model::DocraftText>& node) const;

        /**
         * @brief Returns the list of built-in font names.
         * @return List of font names.
         */
        static std::list<std::string> default_fonts();

        /**
         * @brief Destructor.
         */
        ~DocraftFontApplier();
        /**
         * @brief Returns the backend-registered name for a font.
         * @param name Font family name.
         * @return Backend-registered font name, or nullptr if missing.
         */
        static const char* get_font_registred_name(const std::string& name);
        /**
         * @brief Creates a font applier for a document context.
         * @param context Document context.
         */
        explicit DocraftFontApplier(const std::shared_ptr<DocraftDocumentContext>& context);
        /**
         * @brief Sets whether a font uses UTF-8 encoding.
         * @param font_name Font name.
         * @param utf8 true if UTF-8 encoding should be used.
         */
        static void set_font_encoding(const std::string& font_name, bool utf8);
        /**
         * @brief Returns whether a font uses UTF-8 encoding.
         * @param font_name Font name.
         * @return true if UTF-8 encoding is enabled.
         */
        [[nodiscard]] bool is_font_utf8_encoding(const std::string& font_name) const;
    private:
        /**
         * @brief Checks whether a font is supported by the backend.
         * @param name Font name.
         * @param encoder Backend encoder name.
         * @return true if supported.
         */
        bool is_font_supported(const std::string &name, const char *encoder) const;
        /**
         * @brief Loads font data into a temporary file and returns its path.
         * @param name Font name.
         * @return Path to the temporary file.
         */
        const char *load_font_data(const std::string &name) const;
        static const inline std::filesystem::path kTempDir = std::filesystem::temp_directory_path();
        static inline std::unordered_map<std::string, std::string> fonts_;
        static inline std::unordered_map<std::string, bool> font_utf8_encoding_;
        static inline std::atomic<int> instance_count_{0};
        std::shared_ptr<DocraftDocumentContext> context_;


    };
}

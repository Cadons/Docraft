#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace docraft::utils {
    /**
     * @brief Raw font data container.
     */
    struct DocraftFontData {
        const unsigned char* data;
        size_t size;
    };

    /**
     * @brief Singleton registry for in-memory and file-based fonts.
     *
     * Stores raw font data so backends can register fonts without re-reading files.
     */
    class DocraftFontRegistry {
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
        const DocraftFontData* get_font(const std::string& name) const;
        /**
         * @brief Returns the list of registered font names.
         * @return Vector of font names.
         */
        std::vector<std::string> registered_font_names() const;

    private:
        std::unordered_map<std::string, DocraftFontData> registry_;
        DocraftFontRegistry() = default;
    };
}

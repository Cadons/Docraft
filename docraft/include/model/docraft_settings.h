#pragma once
#include <string>
#include <vector>
#include "model/docraft_node.h"
namespace docraft::model {
    namespace setting {

        /**
         * @brief Represents an external font that can be used in the document.
         * It contains the name of the font and the path to the font file.
         */
        struct DocraftExternalFont {
            std::string name;// e.g. "normal", "bold", "italic", "bold-italic"
            std::string path;// path to the font file, can be relative or absolute
        };
        /**
         * @brief Represents a font that can be used in the document. It contains the name of the font and a list of external fonts that can be used as fallbacks.
         */
        struct DocraftFont {
            std::string name;
            std::vector<DocraftExternalFont> external_fonts;
        };
    }
    /**
     * @brief Document-level settings node (fonts, etc.).
     *
     * Settings are interpreted by the document context to register fonts
     * and configure rendering behavior.
     */
    class DocraftSettings : public DocraftNode {
    public:
        DocraftSettings() = default;

        ~DocraftSettings() override = default;
        // setters
        /**
         * @brief Adds a font family to the settings.
         * @param font Font family definition.
         */
        void add_font(const setting::DocraftFont &font);
        /**
         * @brief Removes a font family by name.
         * @param name Font family name.
         */
        void remove_font(const std::string &name);
        // getters
        /**
         * @brief Returns the configured font families.
         * @return Vector of font families.
         */
        const std::vector<setting::DocraftFont> &fonts() const;

        /**
         * @brief Draws settings (no-op in most backends).
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    private:
        std::vector<setting::DocraftFont> fonts_;
    };
} // namespace docraft::model

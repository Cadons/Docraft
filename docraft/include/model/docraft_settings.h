#pragma once
#include <string>
#include <vector>
#include "model/docraft_node.h"
#include "model/docraft_page_format.h"
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
         * @brief Sets the page format.
         * @param size Page size.
         * @param orientation Page orientation.
         */
        void set_page_format(DocraftPageSize size, DocraftPageOrientation orientation);
        /**
         * @brief Returns the page size.
         */
        [[nodiscard]] DocraftPageSize page_size() const;
        /**
         * @brief Returns the page orientation.
         */
        [[nodiscard]] DocraftPageOrientation page_orientation() const;
        /**
         * @brief Returns whether a page format override is set.
         */
        [[nodiscard]] bool has_page_format() const;
        /**
         * @brief Sets the header/body/footer ratios.
         */
        void set_section_ratios(float header_ratio, float body_ratio, float footer_ratio);
        /**
         * @brief Returns the header ratio.
         */
        [[nodiscard]] float header_ratio() const;
        /**
         * @brief Returns the body ratio.
         */
        [[nodiscard]] float body_ratio() const;
        /**
         * @brief Returns the footer ratio.
         */
        [[nodiscard]] float footer_ratio() const;
        /**
         * @brief Returns whether section ratios are set.
         */
        [[nodiscard]] bool has_section_ratios() const;

        /**
         * @brief Draws settings (no-op in most backends).
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    private:
        std::vector<setting::DocraftFont> fonts_;
        DocraftPageSize page_size_ = DocraftPageSize::kA4;
        DocraftPageOrientation page_orientation_ = DocraftPageOrientation::kPortrait;
        bool has_page_format_ = false;
        float header_ratio_ = 0.06F;
        float body_ratio_ = 0.88F;
        float footer_ratio_ = 0.06F;
        bool has_section_ratios_ = false;
    };
} // namespace docraft::model

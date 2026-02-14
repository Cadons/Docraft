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
    class DocraftSettings : public DocraftNode {
    public:
        DocraftSettings() = default;

        ~DocraftSettings() override = default;
        // setters
        void add_font(const setting::DocraftFont &font);
        void remove_font(const std::string &name);
        // getters
        const std::vector<setting::DocraftFont> &fonts() const;

        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    private:
        std::vector<setting::DocraftFont> fonts_;
    };
} // namespace docraft::model
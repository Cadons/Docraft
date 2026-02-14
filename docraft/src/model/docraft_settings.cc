#include "model/docraft_settings.h"

#include <algorithm>

namespace docraft::model {
    void DocraftSettings::add_font(const setting::DocraftFont &font) {
        fonts_.push_back(font);
    }

    void DocraftSettings::remove_font(const std::string &name) {
        auto rm =[&name](const setting::DocraftFont &font) { return font.name == name; };
        auto range = std::ranges::remove_if(fonts_, rm);
        fonts_.erase(range.begin(), range.end());
    }

    const std::vector<setting::DocraftFont> &DocraftSettings::fonts() const {
        return fonts_;
    }

    void DocraftSettings::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        // settings node does not have a visual representation, it only holds global settings for the document
    }
}

#include "utils/docraft_font_resolver.h"

#include <algorithm>
#include <cctype>

namespace docraft::utils {
    void DocraftFontResolver::rebuild_index(const std::vector<std::string> &builtin_fonts,
                                            const std::vector<std::string> &registered_fonts) {
        index_.clear();
        auto add_font = [&](const std::string &font) {
            const auto parsed = parse_font_name(font);
            auto &variants = index_[parsed.family];
            if (parsed.bold && parsed.italic) {
                if (variants.bold_italic.empty()) variants.bold_italic = font;
            } else if (parsed.bold) {
                if (variants.bold.empty()) variants.bold = font;
            } else if (parsed.italic) {
                if (variants.italic.empty()) variants.italic = font;
            } else {
                if (variants.regular.empty()) variants.regular = font;
            }
        };
        for (const auto &font : builtin_fonts) add_font(font);
        for (const auto &font : registered_fonts) add_font(font);
    }

    std::string DocraftFontResolver::resolve(const std::string &requested,
                                             docraft::model::TextStyle style) const {
        const auto parsed_request = parse_font_name(requested);
        auto it = index_.find(parsed_request.family);
        if (it == index_.end()) {
            return requested;
        }
        const auto &variants = it->second;

        switch (style) {
            case docraft::model::TextStyle::kBoldItalic:
                if (!variants.bold_italic.empty()) return variants.bold_italic;
                if (!variants.bold.empty()) return variants.bold;
                if (!variants.italic.empty()) return variants.italic;
                if (!variants.regular.empty()) return variants.regular;
                break;
            case docraft::model::TextStyle::kBold:
                if (!variants.bold.empty()) return variants.bold;
                if (!variants.regular.empty()) return variants.regular;
                break;
            case docraft::model::TextStyle::kItalic:
                if (!variants.italic.empty()) return variants.italic;
                if (!variants.regular.empty()) return variants.regular;
                break;
            default:
                if (!variants.regular.empty()) return variants.regular;
                break;
        }
        return requested;
    }

    DocraftFontResolver::ParsedFont DocraftFontResolver::parse_font_name(const std::string &name) {
        struct Suffix {
            const char *text;
            bool bold;
            bool italic;
        };
        static const Suffix kSuffixes[] = {
            {"-bolditalic", true, true},
            {"-bold-italic", true, true},
            {"-boldoblique", true, true},
            {"-bold-oblique", true, true},
            {"-bold italic", true, true},
            {"-bold oblique", true, true},
            {"-bold", true, false},
            {"-italic", false, true},
            {"-oblique", false, true},
            {" bolditalic", true, true},
            {" bold italic", true, true},
            {" boldoblique", true, true},
            {" bold oblique", true, true},
            {" bold", true, false},
            {" italic", false, true},
            {" oblique", false, true},
        };

        ParsedFont parsed;
        parsed.family = name;
        std::string lowered = to_lower(name);
        for (const auto &suffix : kSuffixes) {
            const std::string suffix_text = suffix.text;
            if (lowered.size() >= suffix_text.size() &&
                lowered.compare(lowered.size() - suffix_text.size(), suffix_text.size(), suffix_text) == 0) {
                parsed.bold = suffix.bold;
                parsed.italic = suffix.italic;
                parsed.family = trim_trailing_separators(name.substr(0, name.size() - suffix_text.size()));
                return parsed;
            }
        }
        return parsed;
    }

    std::string DocraftFontResolver::to_lower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    }

    std::string DocraftFontResolver::trim_trailing_separators(std::string value) {
        while (!value.empty() && (value.back() == '-' || value.back() == ' ')) {
            value.pop_back();
        }
        return value;
    }
} // docraft::utils

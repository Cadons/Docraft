#include "generic/docraft_font_applier.h"
#include "docraft_document_context.h"
#include "model/docraft_text.h"

#include <hpdf.h>
#define BOLD_FONT "-Bold"
#define ITALIC_FONT "-Italic"
#define OBLIQUE_FONT "-Oblique"
#define BOLD_ITALIC_FONT "-BoldItalic"
#define BOLD_OBLIQUE_FONT "-BoldOblique"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "utils/docraft_font_registry.h"
#include "fonts.h"
#include "utils/docraft_logger.h"

namespace docraft::generic {
    std::list<std::string> DocraftFontApplier::default_fonts() {
        std::list<std::string> fonts;
        /*
        * Libharu supports these fonts
        *   - Courie
            - Courier-Bold
            - Courier-Oblique
            - Courier-BoldOblique
            - Helvetica
            - Helvetica-Bold
            - Helvetica-Oblique
            - Helvetica-BoldOblique
            - Times-Roman
            - Times-Bold
            - Times-Italic
            - Times-BoldItalic
            - Symbol
            - ZapfDingbats
        */
        fonts.emplace_back("Courier");
        fonts.emplace_back("Courier-Bold");
        fonts.emplace_back("Courier-Oblique");
        fonts.emplace_back("Courier-BoldOblique");
        fonts.emplace_back("Helvetica");
        fonts.emplace_back("Helvetica-Bold");
        fonts.emplace_back("Helvetica-Oblique");
        fonts.emplace_back("Helvetica-BoldOblique");
        fonts.emplace_back("Times-Roman");
        fonts.emplace_back("Times-Bold");
        fonts.emplace_back("Times-Italic");
        fonts.emplace_back("Times-BoldItalic");
        fonts.emplace_back("Symbol");
        fonts.emplace_back("ZapfDingbats");
        return fonts;
    }

    DocraftFontApplier::~DocraftFontApplier() {
        try {
            for (const auto &pair: fonts_) {
                const auto &font_name = pair.first;
                auto temp_path = kTempDir / (font_name + ".ttf");
                if (std::filesystem::exists(temp_path)) {
                    std::filesystem::remove(temp_path);
                }
            }
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Error cleaning up temporary font files: " << e.what() << std::endl;
        }
    }

    DocraftFontApplier::DocraftFontApplier(const std::shared_ptr<DocraftDocumentContext> &context) : context_(context) {
        docraft_register_fonts();
        //load default fonts
        for (const auto &font: default_fonts()) {
            load_font_data(font, nullptr);
        }
    }

    bool DocraftFontApplier::is_font_supported(HPDF_Doc pdf, const std::string &name) {
        // Attempt to get the font
        if (!pdf)
            load_font_data(name, pdf); //load custom fonts if are not loaded
        //check if font is in the resources
        if (utils::DocraftFontRegistry::instance().get_font(name) == nullptr) {
            std::cerr << "Font " << name << " not found in the resources" << std::endl;
            return false;
        }
        HPDF_Font font = HPDF_GetFont(pdf, fonts_[name].c_str(), nullptr);

        // Check if the last operation caused an error
        if (!font || HPDF_GetError(pdf) != HPDF_OK) {
            HPDF_ResetError(pdf); // Clear the error so the document remains usable
            return false;
        }
        return true;
    }

      const char *DocraftFontApplier::load_font_data(const std::string &name, HPDF_Doc pdf) {
        // If already mapped, reuse it
        auto it_existing = fonts_.find(name);
        if (it_existing != fonts_.end() && !it_existing->second.empty()) {
            return it_existing->second.c_str();
        }

        auto &registry = utils::DocraftFontRegistry::instance();

        // Built-in fonts: Haru resolves by the same name; no PDF needed here.
        if (pdf == nullptr) {
            fonts_.insert({name, name});
            registry.register_font(name, nullptr, 0);
            font_utf8_encoding_.insert({name, false});
            return fonts_.at(name).c_str();
        }

        const auto *font_resource = registry.get_font(name);
        if (!font_resource || !font_resource->data || font_resource->size == 0) {
            LOG_ERROR("Font resource not found or invalid for font: " + name);
            return nullptr;
        }

        const auto temp_path = kTempDir / (name + ".ttf");

        // Ensure the temp file exists (Haru loads from file path)
        if (!std::filesystem::exists(temp_path)) {
            std::ofstream ofs(temp_path, std::ios::binary);
            if (!ofs) {
                LOG_ERROR("Cannot open temp font file for writing: " + temp_path.string());
                return nullptr;
            }
            ofs.write(reinterpret_cast<const char *>(font_resource->data), font_resource->size);
            ofs.close();
        }

        // IMPORTANT: Always load/register into THIS HPDF_Doc (even if file already existed)
        const char *internal_name = HPDF_LoadTTFontFromFile(pdf, temp_path.string().c_str(), HPDF_TRUE);
        if (!internal_name || HPDF_GetError(pdf) != HPDF_OK) {
           LOG_ERROR("Cannot load internal font file: " + temp_path.string());
            HPDF_ResetError(pdf);
            return nullptr;
        }

        fonts_.insert({name, internal_name});
        font_utf8_encoding_.insert({name, true}); // custom fonts are UTF-8 encoded
        return fonts_.at(name).c_str();
    }


    void DocraftFontApplier::configure_color(HPDF_Doc pdf, HPDF_Page page,
                                             const std::shared_ptr<model::DocraftText> &node) {
        auto rgb = node->color().toRGB();
        float r = rgb.r;
        float g = rgb.g;
        float b = rgb.b;
        // set text fill color
        HPDF_ExtGState ext = HPDF_CreateExtGState(pdf);
        HPDF_ExtGState_SetAlphaFill(ext, rgb.a);
        HPDF_Page_SetExtGState(page, ext);
        HPDF_Page_SetRGBFill(page, r, g, b);
    }

    void DocraftFontApplier::apply_font(
        const std::shared_ptr<model::DocraftText> &node) {
        auto *page = context_->page();
        auto *pdf = context_->pdf_doc();
        if (!pdf || !page) {
            // context_ not ready — avoid HPDF calls
            return;
        }
        const std::string base_name = node->font_name();
        // ensure base font data is loaded once
        auto font_name=load_font_data(base_name, pdf);

        std::string selected = base_name;
        std::vector<std::string> candidates;

        switch (node->style()) {
            case model::TextStyle::kBold:
                if (!base_name.ends_with(BOLD_FONT)) {
                    candidates = {base_name + BOLD_FONT, base_name};
                }
                break;
            case model::TextStyle::kItalic:
                if (!base_name.ends_with(ITALIC_FONT)) {
                    candidates = {base_name + OBLIQUE_FONT, base_name + ITALIC_FONT, base_name};
                }
                break;
            case model::TextStyle::kBoldItalic:
                if (!base_name.ends_with(BOLD_ITALIC_FONT) && !base_name.ends_with(BOLD_OBLIQUE_FONT)) {
                    candidates = {base_name + BOLD_OBLIQUE_FONT, base_name + BOLD_ITALIC_FONT, base_name};
                }
                break;
            default:
                candidates = {base_name};
                break;
        }

        for (const auto &cand: candidates) {
            if (cand != base_name) load_font_data(cand, pdf);
            if (is_font_supported(pdf, cand)) {
                selected = cand;
                break;
            }
        }

        node->set_font_name(selected);
        char *encoder = const_cast<char *>("UTF-8");

        if (font_utf8_encoding_[selected]) {
            for (const auto &default_font: default_fonts()) {
                if (node->font_name() == default_font) {
                    encoder = nullptr;
                    break;
                }
            }
        } else {
            encoder = nullptr;
        }
        HPDF_Font font = HPDF_GetFont(pdf, fonts_[node->font_name()].c_str(), encoder);
        HPDF_Page_SetFontAndSize(page, font, node->font_size());
        configure_color(pdf, page, node);
    }

    void DocraftFontApplier::set_font_encoding(const std::string &font_name, bool utf8) {
        font_utf8_encoding_.insert({font_name, utf8});
    }

    bool DocraftFontApplier::is_font_utf8_encoding(const std::string &font_name) const {
        if (!font_utf8_encoding_.contains(font_name)) return false;
        return font_utf8_encoding_.at(font_name);
    }
    const char* DocraftFontApplier::get_font_registred_name(const std::string& name) {
        auto it = fonts_.find(name);
        if (it != fonts_.end()) {
            return it->second.c_str();
        }
        return nullptr;
    }
} // docraft

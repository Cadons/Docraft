#include "generic/docraft_font_applier.h"
#include "docraft_document_context.h"
#include "model/docraft_text.h"

#include <algorithm>
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include "utils/docraft_font_registry.h"
#include "utils/docraft_font_resolver.h"
#include "fonts.h"
#include "utils/docraft_logger.h"

namespace docraft::generic {
    namespace {
        // Cache built-in fonts once to avoid repeated list construction.
        const std::vector<std::string> &builtin_fonts() {
            static const std::vector<std::string> fonts = [] {
                std::vector<std::string> values;
                auto list = DocraftFontApplier::default_fonts();
                values.assign(list.begin(), list.end());
                return values;
            }();
            return fonts;
        }
    } // namespace
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
        if (instance_count_.fetch_sub(1) != 1) {
            return;
        }
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
        instance_count_.fetch_add(1);
        docraft_register_fonts();
        //load default fonts
        for (const auto &font: default_fonts()) {
            load_font_data(font);
        }
    }

    bool DocraftFontApplier::is_font_supported(const std::string &name, const char *encoder) {
        auto backend = context_->rendering_backend();
        if (!backend) {
            return false;
        }
        auto &registry = utils::DocraftFontRegistry::instance();
        const auto &fonts = builtin_fonts();
        const bool is_builtin = std::find(fonts.begin(), fonts.end(), name) != fonts.end();
        if (!is_builtin && registry.get_font(name) == nullptr) {
            std::cerr << "Font " << name << " not found in the resources" << std::endl;
            return false;
        }
        auto it = fonts_.find(name);
        if (it == fonts_.end() || it->second.empty()) {
            return false;
        }
        return backend->can_use_font(it->second, encoder);
    }

    const char *DocraftFontApplier::load_font_data(const std::string &name) {
        // If already mapped, reuse it
        auto it_existing = fonts_.find(name);
        if (it_existing != fonts_.end() && !it_existing->second.empty()) {
            return it_existing->second.c_str();
        }

        auto &registry = utils::DocraftFontRegistry::instance();

        const auto &fonts = builtin_fonts();
        const bool is_builtin = std::find(fonts.begin(), fonts.end(), name) != fonts.end();
        // Built-in fonts: resolve by the same name.
        if (is_builtin) {
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

        // IMPORTANT: Always load/register into THIS backend (even if file already existed)
        auto backend = context_->rendering_backend();
        if (!backend) {
            return nullptr;
        }
        const char *internal_name = backend->register_ttf_font_from_file(temp_path.string(), true);
        if (!internal_name) {
            LOG_ERROR("Cannot load internal font file: " + temp_path.string());
            return nullptr;
        }

        fonts_.insert({name, internal_name});
        font_utf8_encoding_.insert({name, true}); // custom fonts are UTF-8 encoded
        return fonts_.at(name).c_str();
    }


    void DocraftFontApplier::apply_font(
        const std::shared_ptr<model::DocraftText> &node) {
        auto backend = context_->rendering_backend();
        if (!backend) {
            return;
        }
        const std::string base_name = node->font_name();
        utils::DocraftFontResolver resolver;
        resolver.rebuild_index(builtin_fonts(), utils::DocraftFontRegistry::instance().registered_font_names());
        std::string selected = resolver.resolve(base_name, node->style());
        const char *internal_name = load_font_data(selected);
        if (!internal_name || !is_font_supported(selected, nullptr)) {
            // Final fallback to a known built-in font.
            load_font_data("Helvetica");
            node->set_font_name("Helvetica");
            selected = "Helvetica";
        } else {
            node->set_font_name(selected);
        }
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
        auto it = fonts_.find(node->font_name());
        if (it == fonts_.end() || it->second.empty()) {
            return;
        }
        backend->set_font(it->second, node->font_size(), encoder);
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

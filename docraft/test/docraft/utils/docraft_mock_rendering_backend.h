#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "docraft/backend/docraft_rendering_backend.h"

namespace docraft::test::utils {
    struct MockBackendSharedState {
        struct Config {
            float page_width = 100.0F;
            float page_height = 100.0F;
            float text_width_factor = 5.0F;
            std::size_t initial_pages = 1;
            std::string extension = ".pdf";
            bool can_use_font = true;
            bool supports_line_backend = true;
            bool supports_text_backend = true;
            bool supports_shape_backend = true;
            bool supports_image_backend = true;
            bool supports_page_backend = true;
            bool supports_output_backend = true;
            bool supports_font_backend = true;
            bool supports_metadata_backend = true;
            bool strict_page_lifecycle = true;
            bool require_text_scope = false;
        };

        explicit MockBackendSharedState(Config cfg)
            : config(std::move(cfg)) {
            pages = config.initial_pages > 0 ? config.initial_pages : 1;
        }

        void ensure_supported(bool supported, const char *message) const {
            if (!supported) {
                throw std::runtime_error(message);
            }
        }

        void ensure_page_available() const {
            if (!config.strict_page_lifecycle) {
                return;
            }
            ensure_supported(config.supports_page_backend, "Page backend capability not supported");
            if (pages == 0 || current_page >= pages) {
                throw std::runtime_error("No valid current page");
            }
        }

        void ensure_text_scope_if_required() const {
            if (config.require_text_scope && !text_scope_active) {
                throw std::runtime_error("Text scope required before drawing text");
            }
        }

        Config config;
        std::size_t pages = 1;
        std::size_t current_page = 0;
        mutable bool text_scope_active = false;
        mutable int line_count = 0;
        mutable std::string last_saved_path;
        std::unordered_set<std::string> registered_fonts;
    };

    class MockLineBackend final : public backend::IDocraftLineRenderingBackend {
    public:
        explicit MockLineBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        void set_stroke_color(float, float, float) const override {
            state_->ensure_supported(state_->config.supports_line_backend, "Line backend capability not supported");
            state_->ensure_page_available();
        }

        void set_line_width(float) const override {
            state_->ensure_supported(state_->config.supports_line_backend, "Line backend capability not supported");
            state_->ensure_page_available();
        }

        void draw_line(float, float, float, float) const override {
            state_->ensure_supported(state_->config.supports_line_backend, "Line backend capability not supported");
            state_->ensure_page_available();
            ++state_->line_count;
        }

    private:
        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockTextBackend final : public backend::IDocraftTextRenderingBackend {
    public:
        explicit MockTextBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        void begin_text() const override {
            state_->ensure_supported(state_->config.supports_text_backend, "Text backend capability not supported");
            state_->ensure_page_available();
            if (state_->text_scope_active) {
                throw std::runtime_error("Text scope already active");
            }
            state_->text_scope_active = true;
        }

        void end_text() const override {
            state_->ensure_supported(state_->config.supports_text_backend, "Text backend capability not supported");
            if (!state_->text_scope_active) {
                throw std::runtime_error("Text scope not active");
            }
            state_->text_scope_active = false;
        }

        void draw_text(const std::string &, float, float) const override {
            state_->ensure_supported(state_->config.supports_text_backend, "Text backend capability not supported");
            state_->ensure_page_available();
            state_->ensure_text_scope_if_required();
        }

        void set_text_color(float, float, float) const override {
            state_->ensure_supported(state_->config.supports_text_backend, "Text backend capability not supported");
            state_->ensure_page_available();
        }

        void draw_text_matrix(const std::string &, float, float, float, float, float, float) const override {
            state_->ensure_supported(state_->config.supports_text_backend, "Text backend capability not supported");
            state_->ensure_page_available();
            state_->ensure_text_scope_if_required();
        }

        float measure_text_width(const std::string &text) const override {
            state_->ensure_supported(state_->config.supports_text_backend, "Text backend capability not supported");
            return static_cast<float>(text.size()) * state_->config.text_width_factor;
        }

    private:
        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockShapeBackend final : public backend::IDocraftShapeRenderingBackend {
    public:
        explicit MockShapeBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        void save_state() const override { require(); }
        void restore_state() const override { require(); }
        void set_fill_color(float, float, float) const override { require(); }
        void set_fill_alpha(float) const override { require(); }
        void set_stroke_alpha(float) const override { require(); }
        void draw_rectangle(float, float, float, float) const override { require(); }
        void draw_circle(float, float, float) const override { require(); }
        void draw_polygon(const std::vector<model::DocraftPoint> &) const override { require(); }
        void fill() const override { require(); }
        void stroke() const override { require(); }
        void fill_stroke() const override { require(); }

    private:
        void require() const {
            state_->ensure_supported(state_->config.supports_shape_backend, "Shape backend capability not supported");
            state_->ensure_page_available();
        }

        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockImageBackend final : public backend::IDocraftImageRenderingBackend {
    public:
        explicit MockImageBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        void draw_png_image(const std::string &, float, float, float, float) const override { require(); }

        void draw_png_image_from_memory(const unsigned char *, std::size_t, float, float, float, float) const override {
            require();
        }

        void draw_jpeg_image(const std::string &, float, float, float, float) const override { require(); }

        void draw_jpeg_image_from_memory(const unsigned char *, std::size_t, float, float, float,
                                         float) const override {
            require();
        }

        void draw_raw_rgb_image(const std::string &, int, int, float, float, float, float) const override { require(); }

        void draw_raw_rgb_image_from_memory(const unsigned char *, int, int, float, float, float,
                                            float) const override {
            require();
        }

    private:
        void require() const {
            state_->ensure_supported(state_->config.supports_image_backend, "Image backend capability not supported");
            state_->ensure_page_available();
        }

        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockPageBackend final : public backend::IDocraftPageRenderingBackend {
    public:
        explicit MockPageBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        float page_width() const override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            return state_->config.page_width;
        }

        float page_height() const override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            return state_->config.page_height;
        }

        void add_new_page() override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            ++state_->pages;
            state_->current_page = state_->pages - 1;
        }

        void move_to_next_page() override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            if (state_->current_page + 1 >= state_->pages) {
                throw std::runtime_error("Already at the last page");
            }
            ++state_->current_page;
        }

        void go_to_page(std::size_t page_number) override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            if (page_number >= state_->pages) {
                throw std::runtime_error("Invalid page number");
            }
            state_->current_page = page_number;
        }

        void go_to_first_page() override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            if (state_->pages == 0) {
                throw std::runtime_error("No pages");
            }
            state_->current_page = 0;
        }

        void go_to_previous_page() override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            if (state_->current_page == 0) {
                throw std::runtime_error("Already at first page");
            }
            --state_->current_page;
        }

        void go_to_last_page() override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            if (state_->pages == 0) {
                throw std::runtime_error("No pages");
            }
            state_->current_page = state_->pages - 1;
        }

        void set_page_format(model::DocraftPageSize, model::DocraftPageOrientation) override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
        }

        std::size_t current_page_number() const override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            return state_->current_page + 1;
        }

        std::size_t total_page_count() const override {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            return state_->pages;
        }

    private:
        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockOutputBackend final : public backend::IDocraftOutputBackend {
    public:
        explicit MockOutputBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        void save_to_file(const std::string &path) const override {
            state_->ensure_supported(state_->config.supports_output_backend, "Output backend capability not supported");
            state_->last_saved_path = path;
        }

        [[nodiscard]] std::string file_extension() const override {
            state_->ensure_supported(state_->config.supports_output_backend, "Output backend capability not supported");
            return state_->config.extension;
        }

    private:
        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockFontBackend final : public backend::IDocraftFontBackend {
    public:
        explicit MockFontBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        const char *register_ttf_font_from_file(const std::string &path, bool) const override {
            if (!state_->config.supports_font_backend || path.empty()) {
                return nullptr;
            }
            const auto [it, inserted] = state_->registered_fonts.insert(path);
            (void) inserted;
            return it->c_str();
        }

        bool can_use_font(const std::string &internal_name, const char *) const override {
            if (!state_->config.supports_font_backend || !state_->config.can_use_font) {
                return false;
            }
            if (internal_name == "Helvetica") {
                return true;
            }
            return state_->registered_fonts.contains(internal_name);
        }

        void set_font(const std::string &, float, const char *) const override {
            state_->ensure_supported(state_->config.supports_font_backend, "Font backend capability not supported");
        }

    private:
        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockMetadataBackend final : public backend::IDocraftMetadataBackend {
    public:
        explicit MockMetadataBackend(std::shared_ptr<MockBackendSharedState> state) : state_(std::move(state)) {
        }

        void set_document_metadata(const DocraftDocumentMetadata &) override {
            state_->ensure_supported(state_->config.supports_metadata_backend,
                                     "Metadata backend capability not supported");
        }

    private:
        std::shared_ptr<MockBackendSharedState> state_;
    };

    class MockRenderingBackend : public backend::IDocraftBackend {
    public:
        using Config = MockBackendSharedState::Config;

        MockRenderingBackend() : MockRenderingBackend(Config{}) {
        }

        explicit MockRenderingBackend(Config config)
            : state_(std::make_shared<MockBackendSharedState>(std::move(config))),
              line_backend_(std::make_unique<MockLineBackend>(state_)),
              text_backend_(std::make_unique<MockTextBackend>(state_)),
              shape_backend_(std::make_unique<MockShapeBackend>(state_)),
              image_backend_(std::make_unique<MockImageBackend>(state_)),
              page_backend_(std::make_unique<MockPageBackend>(state_)),
              output_backend_(std::make_unique<MockOutputBackend>(state_)),
              font_backend_(std::make_unique<MockFontBackend>(state_)),
              metadata_backend_(std::make_unique<MockMetadataBackend>(state_)) {
        }

        [[nodiscard]] const backend::IDocraftLineRenderingBackend *line_rendering() const override {
            return state_->config.supports_line_backend ? line_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftLineRenderingBackend *edit_line_rendering() override {
            return state_->config.supports_line_backend ? line_backend_.get() : nullptr;
        }

        [[nodiscard]] const backend::IDocraftTextRenderingBackend *text_rendering() const override {
            return state_->config.supports_text_backend ? text_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftTextRenderingBackend *edit_text_rendering() override {
            return state_->config.supports_text_backend ? text_backend_.get() : nullptr;
        }

        [[nodiscard]] const backend::IDocraftShapeRenderingBackend *shape_rendering() const override {
            return state_->config.supports_shape_backend ? shape_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftShapeRenderingBackend *edit_shape_rendering() override {
            return state_->config.supports_shape_backend ? shape_backend_.get() : nullptr;
        }

        [[nodiscard]] const backend::IDocraftImageRenderingBackend *image_rendering() const override {
            return state_->config.supports_image_backend ? image_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftImageRenderingBackend *edit_image_rendering() override {
            return state_->config.supports_image_backend ? image_backend_.get() : nullptr;
        }

        [[nodiscard]] const backend::IDocraftPageRenderingBackend *page_rendering() const override {
            return state_->config.supports_page_backend ? page_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftPageRenderingBackend *edit_page_rendering() override {
            return state_->config.supports_page_backend ? page_backend_.get() : nullptr;
        }

        [[nodiscard]] const backend::IDocraftOutputBackend *output_backend() const override {
            return state_->config.supports_output_backend ? output_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftOutputBackend *edit_output_backend() override {
            return state_->config.supports_output_backend ? output_backend_.get() : nullptr;
        }

        [[nodiscard]] const backend::IDocraftFontBackend *font_backend() const override {
            return state_->config.supports_font_backend ? font_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftFontBackend *edit_font_backend() override {
            return state_->config.supports_font_backend ? font_backend_.get() : nullptr;
        }

        [[nodiscard]] const backend::IDocraftMetadataBackend *metadata_backend() const override {
            return state_->config.supports_metadata_backend ? metadata_backend_.get() : nullptr;
        }

        [[nodiscard]] backend::IDocraftMetadataBackend *edit_metadata_backend() override {
            return state_->config.supports_metadata_backend ? metadata_backend_.get() : nullptr;
        }

        void set_current_page(std::size_t one_based_page_number) {
            state_->ensure_supported(state_->config.supports_page_backend, "Page backend capability not supported");
            state_->current_page = one_based_page_number > 0 ? one_based_page_number - 1 : 0;
            if (state_->current_page >= state_->pages) {
                state_->current_page = state_->pages - 1;
            }
        }

        std::size_t total_page_count() const {
            return page_backend_->total_page_count();
        }

        std::size_t current_page_number() const {
            return page_backend_->current_page_number();
        }

        void begin_text() const {
            text_backend_->begin_text();
        }

        void draw_text(const std::string &text, float x, float y) const {
            text_backend_->draw_text(text, x, y);
        }

        void end_text() const {
            text_backend_->end_text();
        }

        [[nodiscard]] int line_count() const {
            return state_->line_count;
        }

        [[nodiscard]] const std::string &last_saved_path() const {
            return state_->last_saved_path;
        }

    private:
        std::shared_ptr<MockBackendSharedState> state_;
        std::unique_ptr<MockLineBackend> line_backend_;
        std::unique_ptr<MockTextBackend> text_backend_;
        std::unique_ptr<MockShapeBackend> shape_backend_;
        std::unique_ptr<MockImageBackend> image_backend_;
        std::unique_ptr<MockPageBackend> page_backend_;
        std::unique_ptr<MockOutputBackend> output_backend_;
        std::unique_ptr<MockFontBackend> font_backend_;
        std::unique_ptr<MockMetadataBackend> metadata_backend_;
    };

    class MockBackendProvidersFactory final : public backend::IDocraftBackendProvidersFactory {
    public:
        explicit MockBackendProvidersFactory(std::shared_ptr<MockRenderingBackend> backend)
            : backend_(std::move(backend)) {
        }

        [[nodiscard]] backend::DocraftBackendProviders create_backend_providers() const override {
            return {
                .rendering_provider = backend_,
                .resource_provider = backend_,
                .lifecycle_provider = backend_
            };
        }

    private:
        std::shared_ptr<MockRenderingBackend> backend_;
    };
} // namespace docraft::test::utils

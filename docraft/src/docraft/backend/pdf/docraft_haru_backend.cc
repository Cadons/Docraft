/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "docraft/backend/pdf/docraft_haru_backend.h"

#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <hpdf.h>

#include "docraft/docraft_document_metadata.h"

namespace {
    struct HPDFErrorMap {
        std::unordered_map<std::string, std::string> map;
        HPDFErrorMap() {
            map = {
                { "1001", "Internal error. The consistency of the data was lost." },
                { "1002", "Internal error. The consistency of the data was lost." },
                { "1003", "Internal error. The consistency of the data was lost." },
                { "1004", "The length of the data exceeds HPDF_LIMIT_MAX_STRING_LEN." },
                { "1005", "Cannot get a palette data from PNG image." },
                { "1006", "Reserved/unknown HPDF error." },
                { "1007", "The count of elements of a dictionary exceeds HPDF_LIMIT_MAX_DICT_ELEMENT." },
                { "1008", "Internal error. The consistency of the data was lost." },
                { "1009", "Internal error. The consistency of the data was lost." },
                { "100a", "Internal error. The consistency of the data was lost." },
                { "100b", "HPDF_SetPermission() or HPDF_SetEncryptMode() was called before a password is set." },
                { "100c", "Internal error. The consistency of the data was lost." },
                { "100d", "Reserved/unknown HPDF error." },
                { "100e", "Tried to register a font that has been registered." },
                { "100f", "Cannot register a character to the japanese word wrap characters list." },
                { "1010", "Reserved/unknown HPDF error." },
                { "1011", "Tried to set the owner password to NULL or owner and user password are the same." },
                { "1013", "Internal error. The consistency of the data was lost." },
                { "1014", "The depth of the graphics state stack exceeded HPDF_LIMIT_MAX_GSTATE." },
                { "1015", "Memory allocation failed." },
                { "1016", "File processing failed. (A detailed code is set.)" },
                { "1017", "Cannot open a file. (A detailed code is set.)" },
                { "1018", "Reserved/unknown HPDF error." },
                { "1019", "Tried to load a font that has been registered." },
                { "101a", "Invalid font-file format or internal consistency error." },
                { "101b", "Cannot recognize a header of an AFM file." },
                { "101c", "The specified annotation handle is invalid." },
                { "101d", "Reserved/unknown HPDF error." },
                { "101e", "Invalid bit-per-component for mask image." },
                { "101f", "Cannot recognize char-metrics data of an AFM file." },
                { "1020", "Invalid color space or invalid operation for current color space." },
                { "1021", "Invalid compression mode value." },
                { "1022", "An invalid date-time value was set." },
                { "1023", "An invalid destination handle was set." },
                { "1024", "Reserved/unknown HPDF error." },
                { "1025", "An invalid document handle is set." },
                { "1026", "Called a function invalid in the current document state." },
                { "1027", "An invalid encoder handle is set." },
                { "1028", "Invalid combination between font and encoder." },
                { "1029", "Reserved/unknown HPDF error." },
                { "102a", "Reserved/unknown HPDF error." },
                { "102b", "An invalid encoding name is specified." },
                { "102c", "Invalid encryption key length." },
                { "102d", "Invalid font definition data or unsupported font format." },
                { "102e", "Internal error. The consistency of the data was lost." },
                { "102f", "A font with the specified name is not found." },
                { "1030", "Unsupported image format." },
                { "1031", "Unsupported JPEG data." },
                { "1032", "Cannot read a postscript-name from an AFM file." },
                { "1033", "Invalid object or internal consistency error." },
                { "1034", "Internal error. The consistency of the data was lost." },
                { "1035", "Invalid operation (e.g. wrong use of image mask functions)." },
                { "1036", "An invalid outline handle was specified." },
                { "1037", "An invalid page handle was specified." },
                { "1038", "An invalid pages handle was specified." },
                { "1039", "An invalid parameter value was set." },
                { "103a", "Reserved/unknown HPDF error." },
                { "103b", "Invalid PNG image format." },
                { "103c", "Internal error. The consistency of the data was lost." },
                { "103d", "Internal error. The \"_FILE_NAME\" entry for delayed loading is missing." },
                { "103e", "Reserved/unknown HPDF error." },
                { "103f", "Invalid .TTC file format." },
                { "1040", "TTC index parameter exceeded number of included fonts." },
                { "1041", "Cannot read width data from an AFM file." },
                { "1042", "Internal error. The consistency of the data was lost." },
                { "1043", "An error from libpng while loading an image." },
                { "1044", "Internal error. The consistency of the data was lost." },
                { "1045", "Internal error. The consistency of the data was lost." },
                { "1046", "Reserved/unknown HPDF error." },
                { "1047", "Reserved/unknown HPDF error." },
                { "1048", "Reserved/unknown HPDF error." },
                { "1049", "Internal error. The consistency of the data was lost." },
                { "104a", "Internal error. The consistency of the data was lost." },
                { "104b", "Internal error. The consistency of the data was lost." },
                { "104c", "There are no graphics-states to be restored." },
                { "104d", "Internal error. The consistency of the data was lost." },
                { "104e", "The current font is not set." },
                { "104f", "An invalid font handle was specified." },
                { "1050", "An invalid font size was set." },
                { "1051", "Invalid graphics mode." },
                { "1052", "Internal error. The consistency of the data was lost." },
                { "1053", "The specified rotate value is not a multiple of 90." },
                { "1054", "An invalid page size was set." },
                { "1055", "An invalid image handle was set." },
                { "1056", "The specified value is out of range." },
                { "1057", "The specified real value is out of range." },
                { "1058", "Unexpected EOF marker was detected." },
                { "1059", "Internal error. The consistency of the data was lost." },
                { "105a", "Reserved/unknown HPDF error." },
                { "105b", "The length of the specified text is too long." },
                { "105c", "The execution of a function was skipped because of other errors." },
                { "105d", "This TrueType font cannot be embedded (restricted by license)." },
                { "105e", "Unsupported TTF format (invalid cmap)." },
                { "105f", "Unsupported TTF format." },
                { "1060", "Unsupported TTF format (missing table)." },
                { "1061", "Internal error. The consistency of the data was lost." },
                { "1062", "Library not configured to use libpng or internal error." },
                { "1063", "Unsupported JPEG format." },
                { "1064", "Failed to parse .PFB file." },
                { "1065", "Internal error. The consistency of the data was lost." },
                { "1066", "An error occurred while executing a function of zlib." },
                { "1067", "An error returned from zlib." },
                { "1068", "An invalid URI was set." },
                { "1069", "An invalid page layout was set." },
                { "1070", "An invalid page mode was set." },
                { "1071", "An invalid page number style was set." },
                { "1072", "An invalid annotation icon was set." },
                { "1073", "An invalid annotation border style was set." },
                { "1074", "An invalid page direction was set." },
                { "1075", "An invalid font handle was specified." }
            };
        }
        static const HPDFErrorMap& instance() {
            static HPDFErrorMap inst;
            return inst;
        }
    };

    void HPDF_STDCALL error_handler(HPDF_STATUS error_no, HPDF_STATUS, void *) {
        std::ostringstream ss;
        ss << std::hex << error_no;
        const std::string error_no_hex = ss.str();

        const auto &err_map = HPDFErrorMap::instance().map;
        const auto it = err_map.find(error_no_hex);

        if (it != err_map.end()) {
            std::cerr << "error: error_no=0x" << error_no_hex << ", message=" << it->second << std::endl;
        } else {
            std::cerr << "error: error_no=0x" << error_no_hex << std::endl;
        }
    }
} // namespace

namespace docraft::backend::pdf {
    namespace {
        HPDF_PageSizes to_hpdf_size(model::DocraftPageSize size) {
            switch (size) {
                case model::DocraftPageSize::kA3:
                    return HPDF_PAGE_SIZE_A3;
                case model::DocraftPageSize::kA5:
                    return HPDF_PAGE_SIZE_A5;
                case model::DocraftPageSize::kLetter:
                    return HPDF_PAGE_SIZE_LETTER;
                case model::DocraftPageSize::kLegal:
                    return HPDF_PAGE_SIZE_LEGAL;
                case model::DocraftPageSize::kA4:
                default:
                    return HPDF_PAGE_SIZE_A4;
            }
        }

        HPDF_PageDirection to_hpdf_direction(model::DocraftPageOrientation orientation) {
            switch (orientation) {
                case model::DocraftPageOrientation::kLandscape:
                    return HPDF_PAGE_LANDSCAPE;
                case model::DocraftPageOrientation::kPortrait:
                default:
                    return HPDF_PAGE_PORTRAIT;
            }
        }

        void throw_if_hpdf_error(HPDF_STATUS status, const std::string &operation) {
            if (status == HPDF_OK) {
                return;
            }
            std::ostringstream stream;
            stream << operation << " (HPDF status 0x" << std::hex << status << ")";
            throw std::runtime_error(stream.str());
        }

        HPDF_Date to_hpdf_date(const DocraftDocumentMetadata::DateTime &date) {
            return HPDF_Date{
                .year = date.year,
                .month = date.month,
                .day = date.day,
                .hour = date.hour,
                .minutes = date.minutes,
                .seconds = date.seconds,
                .ind = date.ind,
                .off_hour = date.off_hour,
                .off_minutes = date.off_minutes
            };
        }

        void set_info_attr_if_present(HPDF_Doc pdf,
                                      HPDF_InfoType type,
                                      const std::optional<std::string> &value,
                                      const std::string &field_name) {
            if (!value || value->empty()) {
                return;
            }
            const HPDF_STATUS status = HPDF_SetInfoAttr(pdf, type, value->c_str());
            throw_if_hpdf_error(status, "Failed to set PDF metadata '" + field_name + "'");
        }

        void set_info_date_attr_if_present(HPDF_Doc pdf,
                                           HPDF_InfoType type,
                                           const std::optional<DocraftDocumentMetadata::DateTime> &value,
                                           const std::string &field_name) {
            if (!value) {
                return;
            }
            const HPDF_STATUS status = HPDF_SetInfoDateAttr(pdf, type, to_hpdf_date(*value));
            throw_if_hpdf_error(status, "Failed to set PDF metadata '" + field_name + "'");
        }
    } // namespace

    class DocraftHaruBackend::TextHaruBackend : public docraft::backend::IDocraftTextRenderingBackend {
    public:
        explicit TextHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {}

        void begin_text() const override {
            HPDF_Page_BeginText(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void end_text() const override {
            HPDF_Page_EndText(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void draw_text(const std::string &text, float x, float y) const override {
            HPDF_Page_TextOut(owner_.pages_[owner_.internal_current_page_index()], x, y, text.c_str());
        }

        void set_text_color(float r, float g, float b) const override {
            HPDF_Page_SetRGBFill(owner_.pages_[owner_.internal_current_page_index()], r, g, b);
        }

        void draw_text_matrix(const std::string &text,
                              float scale_x,
                              float skew_x,
                              float skew_y,
                              float scale_y,
                              float translate_x,
                              float translate_y) const override {
            HPDF_Page_SetTextMatrix(
                owner_.pages_[owner_.internal_current_page_index()],
                scale_x,
                skew_x,
                skew_y,
                scale_y,
                translate_x,
                translate_y);
            HPDF_Page_ShowText(owner_.pages_[owner_.internal_current_page_index()], text.c_str());
        }

        float measure_text_width(const std::string &text) const override {
            return HPDF_Page_TextWidth(owner_.pages_[owner_.internal_current_page_index()], text.c_str());
        }

    private:
        DocraftHaruBackend &owner_;
    };

    class DocraftHaruBackend::LineHaruBackend : public docraft::backend::IDocraftLineRenderingBackend {
    public:
        explicit LineHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {}

        void set_stroke_color(float r, float g, float b) const override {
            HPDF_Page_SetRGBStroke(owner_.pages_[owner_.internal_current_page_index()], r, g, b);
        }

        void set_line_width(float thickness) const override {
            HPDF_Page_SetLineWidth(owner_.pages_[owner_.internal_current_page_index()], thickness);
        }

        void draw_line(float x1, float y1, float x2, float y2) const override {
            HPDF_Page_MoveTo(owner_.pages_[owner_.internal_current_page_index()], x1, y1);
            HPDF_Page_LineTo(owner_.pages_[owner_.internal_current_page_index()], x2, y2);
            HPDF_Page_Stroke(owner_.pages_[owner_.internal_current_page_index()]);
        }

    private:
        DocraftHaruBackend &owner_;
    };

    class DocraftHaruBackend::ShapeHaruBackend : public docraft::backend::IDocraftShapeRenderingBackend {
    public:
        explicit ShapeHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {}

        void save_state() const override {
            HPDF_Page_GSave(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void restore_state() const override {
            HPDF_Page_GRestore(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void set_fill_color(float r, float g, float b) const override {
            HPDF_Page_SetRGBFill(owner_.pages_[owner_.internal_current_page_index()], r, g, b);
        }

        void set_fill_alpha(float alpha) const override {
            owner_.fill_alpha_ = alpha;
            owner_.apply_alpha_state();
        }

        void set_stroke_alpha(float alpha) const override {
            owner_.stroke_alpha_ = alpha;
            owner_.apply_alpha_state();
        }

        void draw_rectangle(float x, float y, float width, float height) const override {
            HPDF_Page_Rectangle(owner_.pages_[owner_.internal_current_page_index()], x, y, width, height);
        }

        void draw_circle(float center_x, float center_y, float radius) const override {
            HPDF_Page_Circle(owner_.pages_[owner_.internal_current_page_index()], center_x, center_y, radius);
        }

        void draw_polygon(const std::vector<model::DocraftPoint> &points) const override {
            if (points.size() < 2U) {
                return;
            }

            HPDF_Page_MoveTo(owner_.pages_[owner_.internal_current_page_index()], points[0].x, points[0].y);
            for (size_t i = 1; i < points.size(); ++i) {
                HPDF_Page_LineTo(owner_.pages_[owner_.internal_current_page_index()], points[i].x, points[i].y);
            }
            HPDF_Page_ClosePath(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void fill() const override {
            HPDF_Page_Fill(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void stroke() const override {
            HPDF_Page_Stroke(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void fill_stroke() const override {
            HPDF_Page_FillStroke(owner_.pages_[owner_.internal_current_page_index()]);
        }

    private:
        DocraftHaruBackend &owner_;
    };

    class DocraftHaruBackend::ImageHaruBackend : public docraft::backend::IDocraftImageRenderingBackend {
    public:
        explicit ImageHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {}

        void draw_png_image(const std::string& path,
                            float x,
                            float y,
                            float width,
                            float height) const override {
            auto image = HPDF_LoadPngImageFromFile(owner_.pdf_, path.c_str());
            if (!image) {
                throw std::runtime_error("Failed to load PNG image: " + path);
            }
            HPDF_Page_DrawImage(owner_.pages_[owner_.internal_current_page_index()], image, x, y, width, height);
        }

        void draw_png_image_from_memory(const unsigned char* data,
                                        std::size_t size,
                                        float x,
                                        float y,
                                        float width,
                                        float height) const override {
            auto image = HPDF_LoadPngImageFromMem(
                owner_.pdf_,
                reinterpret_cast<const HPDF_BYTE*>(data),
                static_cast<HPDF_UINT>(size));
            if (!image) {
                throw std::runtime_error("Failed to load PNG image from memory");
            }
            HPDF_Page_DrawImage(owner_.pages_[owner_.internal_current_page_index()], image, x, y, width, height);
        }

        void draw_jpeg_image(const std::string& path,
                             float x,
                             float y,
                             float width,
                             float height) const override {
            auto image = HPDF_LoadJpegImageFromFile(owner_.pdf_, path.c_str());
            if (!image) {
                throw std::runtime_error("Failed to load JPEG image: " + path);
            }
            HPDF_Page_DrawImage(owner_.pages_[owner_.internal_current_page_index()], image, x, y, width, height);
        }

        void draw_jpeg_image_from_memory(const unsigned char* data,
                                         std::size_t size,
                                         float x,
                                         float y,
                                         float width,
                                         float height) const override {
            auto image = HPDF_LoadJpegImageFromMem(
                owner_.pdf_,
                reinterpret_cast<const HPDF_BYTE*>(data),
                static_cast<HPDF_UINT>(size));
            if (!image) {
                throw std::runtime_error("Failed to load JPEG image from memory");
            }
            HPDF_Page_DrawImage(owner_.pages_[owner_.internal_current_page_index()], image, x, y, width, height);
        }

        void draw_raw_rgb_image(const std::string& path,
                                int pixel_width,
                                int pixel_height,
                                float x,
                                float y,
                                float width,
                                float height) const override {
            auto image = HPDF_LoadRawImageFromFile(
                owner_.pdf_,
                path.c_str(),
                static_cast<HPDF_UINT>(pixel_width),
                static_cast<HPDF_UINT>(pixel_height),
                HPDF_CS_DEVICE_RGB);
            if (!image) {
                throw std::runtime_error("Failed to load raw RGB image: " + path);
            }
            HPDF_Page_DrawImage(owner_.pages_[owner_.internal_current_page_index()], image, x, y, width, height);
        }

        void draw_raw_rgb_image_from_memory(const unsigned char* data,
                                            int pixel_width,
                                            int pixel_height,
                                            float x,
                                            float y,
                                            float width,
                                            float height) const override {
            constexpr HPDF_UINT bits_per_component = 8;
            auto *image = HPDF_LoadRawImageFromMem(
                owner_.pdf_,
                reinterpret_cast<const HPDF_BYTE*>(data),
                static_cast<HPDF_UINT>(pixel_width),
                static_cast<HPDF_UINT>(pixel_height),
                HPDF_CS_DEVICE_RGB,
                bits_per_component);
            if (!image) {
                throw std::runtime_error("Failed to load raw RGB image from memory");
            }
            HPDF_Page_DrawImage(owner_.pages_[owner_.internal_current_page_index()], image, x, y, width, height);
        }

    private:
        DocraftHaruBackend &owner_;
    };

    class DocraftHaruBackend::PageHaruBackend : public docraft::backend::IDocraftPageRenderingBackend {
    public:
        explicit PageHaruBackend(DocraftHaruBackend &owner) : owner_(owner) {}

        float page_width() const override {
            return HPDF_Page_GetWidth(owner_.pages_[owner_.internal_current_page_index()]);
        }

        float page_height() const override {
            return HPDF_Page_GetHeight(owner_.pages_[owner_.internal_current_page_index()]);
        }

        void add_new_page() override {
            owner_.create_new_page();
        }

        void move_to_next_page() override {
            if (owner_.current_page_number_ + 1 < owner_.pages_.size()) {
                ++owner_.current_page_number_;
                return;
            }
            throw std::runtime_error("Already at the last page, cannot move to next page");
        }

        void go_to_page(std::size_t page_number) override {
            if (page_number < owner_.pages_.size()) {
                owner_.current_page_number_ = page_number;
                return;
            }
            throw std::runtime_error("Invalid page number: " + std::to_string(page_number));
        }

        void go_to_first_page() override {
            if (owner_.pages_.empty()) {
                throw std::runtime_error("No pages in document");
            }
            owner_.current_page_number_ = 0;
        }

        void go_to_previous_page() override {
            if (owner_.current_page_number_ == 0) {
                throw std::runtime_error("Already at the first page, cannot move to previous page");
            }
            --owner_.current_page_number_;
        }

        void go_to_last_page() override {
            if (owner_.pages_.empty()) {
                throw std::runtime_error("No pages in document");
            }
            owner_.current_page_number_ = owner_.pages_.size() - 1;
        }

        void set_page_format(model::DocraftPageSize size,
                             model::DocraftPageOrientation orientation) override {
            owner_.page_size_ = to_hpdf_size(size);
            owner_.page_direction_ = to_hpdf_direction(orientation);
            for (auto &page : owner_.pages_) {
                if (page) {
                    owner_.apply_page_format(page);
                }
            }
        }

        std::size_t current_page_number() const override {
            return owner_.current_page_number_ + 1;
        }

        std::size_t total_page_count() const override {
            return owner_.pages_.size();
        }

    private:
        DocraftHaruBackend &owner_;
    };

    void DocraftHaruBackend::create_new_page() {
        HPDF_Page new_page = HPDF_AddPage(pdf_);
        if (!new_page) {
            throw std::runtime_error("Failed to create a new page");
        }
        apply_page_format(new_page);
        pages_.push_back(new_page);
        current_page_number_ = pages_.size() - 1;
    }

    DocraftHaruBackend::DocraftHaruBackend()
        : text_backend_(std::make_unique<TextHaruBackend>(*this)),
          line_backend_(std::make_unique<LineHaruBackend>(*this)),
          shape_backend_(std::make_unique<ShapeHaruBackend>(*this)),
          image_backend_(std::make_unique<ImageHaruBackend>(*this)),
          page_backend_(std::make_unique<PageHaruBackend>(*this)),
          pdf_(nullptr) {
        pdf_ = HPDF_New(error_handler, nullptr);
        if (!pdf_) {
            throw std::runtime_error("Failed to initialize Haru PDF document");
        }
        HPDF_UseUTFEncodings(pdf_);
        HPDF_SetCurrentEncoder(pdf_, "UTF-8");
        HPDF_SetCompressionMode(pdf_, HPDF_COMP_ALL);
        create_new_page();
    }

    DocraftHaruBackend::~DocraftHaruBackend() {
        if (pdf_) {
            HPDF_Free(pdf_);
            pdf_ = nullptr;
        }
        for (auto &page : pages_) {
            page = nullptr;
        }
        pages_.clear();
    }

    const docraft::backend::IDocraftLineRenderingBackend *DocraftHaruBackend::line_rendering() const {
        return line_backend_.get();
    }

    docraft::backend::IDocraftLineRenderingBackend *DocraftHaruBackend::edit_line_rendering() {
        return line_backend_.get();
    }

    const docraft::backend::IDocraftTextRenderingBackend *DocraftHaruBackend::text_rendering() const {
        return text_backend_.get();
    }

    docraft::backend::IDocraftTextRenderingBackend *DocraftHaruBackend::edit_text_rendering() {
        return text_backend_.get();
    }

    const docraft::backend::IDocraftShapeRenderingBackend *DocraftHaruBackend::shape_rendering() const {
        return shape_backend_.get();
    }

    docraft::backend::IDocraftShapeRenderingBackend *DocraftHaruBackend::edit_shape_rendering() {
        return shape_backend_.get();
    }

    const docraft::backend::IDocraftImageRenderingBackend *DocraftHaruBackend::image_rendering() const {
        return image_backend_.get();
    }

    docraft::backend::IDocraftImageRenderingBackend *DocraftHaruBackend::edit_image_rendering() {
        return image_backend_.get();
    }

    const docraft::backend::IDocraftPageRenderingBackend *DocraftHaruBackend::page_rendering() const {
        return page_backend_.get();
    }

    docraft::backend::IDocraftPageRenderingBackend *DocraftHaruBackend::edit_page_rendering() {
        return page_backend_.get();
    }

    void DocraftHaruBackend::save_to_file(const std::string& path) const {
        HPDF_SaveToFile(pdf_, path.c_str());
    }

    std::string DocraftHaruBackend::file_extension() const {
        return ".pdf";
    }

    const char* DocraftHaruBackend::register_ttf_font_from_file(const std::string& path, bool embed) const {
        const char* result = HPDF_LoadTTFontFromFile(pdf_, path.c_str(), embed ? HPDF_TRUE : HPDF_FALSE);
        if (!result) {
            HPDF_ResetError(pdf_);
        }
        return result;
    }

    bool DocraftHaruBackend::can_use_font(const std::string& internal_name, const char* encoder) const {
        HPDF_Font font = HPDF_GetFont(pdf_, internal_name.c_str(), encoder);
        if (!font || HPDF_GetError(pdf_) != HPDF_OK) {
            HPDF_ResetError(pdf_);
            return false;
        }
        return true;
    }

    void DocraftHaruBackend::set_font(const std::string& internal_name, float size, const char* encoder) const {
        HPDF_Font font = HPDF_GetFont(pdf_, internal_name.c_str(), encoder);
        if (!font) {
            throw std::runtime_error("Failed to resolve font: " + internal_name);
        }
        HPDF_Page_SetFontAndSize(pages_[internal_current_page_index()], font, size);
    }

    void DocraftHaruBackend::set_document_metadata(const DocraftDocumentMetadata &metadata) {
        set_info_date_attr_if_present(pdf_, HPDF_INFO_CREATION_DATE, metadata.creation_date(), "creation_date");
        set_info_date_attr_if_present(pdf_, HPDF_INFO_MOD_DATE, metadata.modification_date(), "modification_date");
        set_info_attr_if_present(pdf_, HPDF_INFO_AUTHOR, metadata.author(), "author");
        set_info_attr_if_present(pdf_, HPDF_INFO_CREATOR, metadata.creator(), "creator");
        set_info_attr_if_present(pdf_, HPDF_INFO_PRODUCER, metadata.producer(), "producer");
        set_info_attr_if_present(pdf_, HPDF_INFO_TITLE, metadata.title(), "title");
        set_info_attr_if_present(pdf_, HPDF_INFO_SUBJECT, metadata.subject(), "subject");
        set_info_attr_if_present(pdf_, HPDF_INFO_KEYWORDS, metadata.keywords(), "keywords");
        set_info_attr_if_present(pdf_, HPDF_INFO_TRAPPED, metadata.trapped(), "trapped");
        set_info_attr_if_present(pdf_, HPDF_INFO_GTS_PDFX, metadata.gts_pdfx(), "gts_pdfx");
    }

    void DocraftHaruBackend::apply_alpha_state() const {
        auto *ext = HPDF_CreateExtGState(pdf_);
        if (ext) {
            HPDF_ExtGState_SetAlphaFill(ext, fill_alpha_);
            HPDF_ExtGState_SetAlphaStroke(ext, stroke_alpha_);
            HPDF_Page_SetExtGState(pages_[internal_current_page_index()], ext);
        }
    }

    size_t DocraftHaruBackend::internal_current_page_index() const {
        return current_page_number_;
    }

    void DocraftHaruBackend::apply_page_format(HPDF_Page page) const {
        HPDF_Page_SetSize(page, page_size_, page_direction_);
    }
} // docraft::backend::pdf

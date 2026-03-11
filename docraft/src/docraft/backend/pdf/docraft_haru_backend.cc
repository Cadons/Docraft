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
                { "1001", "Internal error. The consistency of the data was lost." }, // HPDF_ARRAY_COUNT_ERR
                { "1002", "Internal error. The consistency of the data was lost." }, // HPDF_ARRAY_ITEM_NOT_FOUND
                { "1003", "Internal error. The consistency of the data was lost." }, // HPDF_ARRAY_ITEM_UNEXPECTED_TYPE
                { "1004", "The length of the data exceeds HPDF_LIMIT_MAX_STRING_LEN." }, // HPDF_BINARY_LENGTH_ERR
                { "1005", "Cannot get a palette data from PNG image." }, // HPDF_CANNOT_GET_PALLET
                { "1006", "Reserved/unknown HPDF error." },
                { "1007", "The count of elements of a dictionary exceeds HPDF_LIMIT_MAX_DICT_ELEMENT." }, // HPDF_DICT_COUNT_ERR
                { "1008", "Internal error. The consistency of the data was lost." }, // HPDF_DICT_ITEM_NOT_FOUND
                { "1009", "Internal error. The consistency of the data was lost." }, // HPDF_DICT_ITEM_UNEXPECTED_TYPE
                { "100a", "Internal error. The consistency of the data was lost." }, // HPDF_DICT_STREAM_LENGTH_NOT_FOUND
                { "100b", "HPDF_SetPermission() or HPDF_SetEncryptMode() was called before a password is set." }, // HPDF_DOC_ENCRYPTDICT_NOT_FOUND
                { "100c", "Internal error. The consistency of the data was lost." }, // HPDF_DOC_INVALID_OBJECT
                { "100d", "Reserved/unknown HPDF error." },
                { "100e", "Tried to register a font that has been registered." }, // HPDF_DUPLICATE_REGISTRATION
                { "100f", "Cannot register a character to the japanese word wrap characters list." }, // HPDF_EXCEED_JWW_CODE_NUM_LIMIT
                { "1010", "Reserved/unknown HPDF error." },
                { "1011", "Tried to set the owner password to NULL or owner and user password are the same." }, // HPDF_ENCRYPT_INVALID_PASSWORD
                { "1013", "Internal error. The consistency of the data was lost." }, // HPDF_ERR_UNKNOWN_CLASS
                { "1014", "The depth of the graphics state stack exceeded HPDF_LIMIT_MAX_GSTATE." }, // HPDF_EXCEED_GSTATE_LIMIT
                { "1015", "Memory allocation failed." }, // HPDF_FAILD_TO_ALLOC_MEM
                { "1016", "File processing failed. (A detailed code is set.)" }, // HPDF_FILE_IO_ERROR
                { "1017", "Cannot open a file. (A detailed code is set.)" }, // HPDF_FILE_OPEN_ERROR
                { "1018", "Reserved/unknown HPDF error." },
                { "1019", "Tried to load a font that has been registered." }, // HPDF_FONT_EXISTS
                { "101a", "Invalid font-file format or internal consistency error." }, // HPDF_FONT_INVALID_WIDTHS_TABLE
                { "101b", "Cannot recognize a header of an AFM file." }, // HPDF_INVALID_AFM_HEADER
                { "101c", "The specified annotation handle is invalid." }, // HPDF_INVALID_ANNOTATION
                { "101d", "Reserved/unknown HPDF error." },
                { "101e", "Invalid bit-per-component for mask image." }, // HPDF_INVALID_BIT_PER_COMPONENT
                { "101f", "Cannot recognize char-metrics data of an AFM file." }, // HPDF_INVALID_CHAR_MATRICS_DATA
                { "1020", "Invalid color space or invalid operation for current color space." }, // HPDF_INVALID_COLOR_SPACE
                { "1021", "Invalid compression mode value." }, // HPDF_INVALID_COMPRESSION_MODE
                { "1022", "An invalid date-time value was set." }, // HPDF_INVALID_DATE_TIME
                { "1023", "An invalid destination handle was set." }, // HPDF_INVALID_DESTINATION
                { "1024", "Reserved/unknown HPDF error." },
                { "1025", "An invalid document handle is set." }, // HPDF_INVALID_DOCUMENT
                { "1026", "Called a function invalid in the current document state." }, // HPDF_INVALID_DOCUMENT_STATE
                { "1027", "An invalid encoder handle is set." }, // HPDF_INVALID_ENCODER
                { "1028", "Invalid combination between font and encoder." }, // HPDF_INVALID_ENCODER_TYPE
                { "1029", "Reserved/unknown HPDF error." },
                { "102a", "Reserved/unknown HPDF error." },
                { "102b", "An invalid encoding name is specified." }, // HPDF_INVALID_ENCODING_NAME
                { "102c", "Invalid encryption key length." }, // HPDF_INVALID_ENCRYPT_KEY_LEN
                { "102d", "Invalid font definition data or unsupported font format." }, // HPDF_INVALID_FONTDEF_DATA
                { "102e", "Internal error. The consistency of the data was lost." }, // HPDF_INVALID_FONTDEF_TYPE
                { "102f", "A font with the specified name is not found." }, // HPDF_INVALID_FONT_NAME
                { "1030", "Unsupported image format." }, // HPDF_INVALID_IMAGE
                { "1031", "Unsupported JPEG data." }, // HPDF_INVALID_JPEG_DATA
                { "1032", "Cannot read a postscript-name from an AFM file." }, // HPDF_INVALID_N_DATA
                { "1033", "Invalid object or internal consistency error." }, // HPDF_INVALID_OBJECT
                { "1034", "Internal error. The consistency of the data was lost." }, // HPDF_INVALID_OBJ_ID
                { "1035", "Invalid operation (e.g. wrong use of image mask functions)." }, // HPDF_INVALID_OPERATION
                { "1036", "An invalid outline handle was specified." }, // HPDF_INVALID_OUTLINE
                { "1037", "An invalid page handle was specified." }, // HPDF_INVALID_PAGE
                { "1038", "An invalid pages handle was specified." }, // HPDF_INVALID_PAGES
                { "1039", "An invalid parameter value was set." }, // HPDF_INVALID_PARAMETER
                { "103a", "Reserved/unknown HPDF error." },
                { "103b", "Invalid PNG image format." }, // HPDF_INVALID_PNG_IMAGE
                { "103c", "Internal error. The consistency of the data was lost." }, // HPDF_INVALID_STREAM
                { "103d", "Internal error. The \"_FILE_NAME\" entry for delayed loading is missing." }, // HPDF_MISSING_FILE_NAME_ENTRY
                { "103e", "Reserved/unknown HPDF error." },
                { "103f", "Invalid .TTC file format." }, // HPDF_INVALID_TTC_FILE
                { "1040", "TTC index parameter exceeded number of included fonts." }, // HPDF_INVALID_TTC_INDEX
                { "1041", "Cannot read width data from an AFM file." }, // HPDF_INVALID_WX_DATA
                { "1042", "Internal error. The consistency of the data was lost." }, // HPDF_ITEM_NOT_FOUND
                { "1043", "An error from libpng while loading an image." }, // HPDF_LIBPNG_ERROR
                { "1044", "Internal error. The consistency of the data was lost." }, // HPDF_NAME_INVALID_VALUE
                { "1045", "Internal error. The consistency of the data was lost." }, // HPDF_NAME_OUT_OF_RANGE
                { "1046", "Reserved/unknown HPDF error." },
                { "1047", "Reserved/unknown HPDF error." },
                { "1048", "Reserved/unknown HPDF error." },
                { "1049", "Internal error. The consistency of the data was lost." }, // HPDF_PAGES_MISSING_KIDS_ENTRY
                { "104a", "Internal error. The consistency of the data was lost." }, // HPDF_PAGE_CANNOT_FIND_OBJECT
                { "104b", "Internal error. The consistency of the data was lost." }, // HPDF_PAGE_CANNOT_GET_ROOT_PAGES
                { "104c", "There are no graphics-states to be restored." }, // HPDF_PAGE_CANNOT_RESTORE_GSTATE
                { "104d", "Internal error. The consistency of the data was lost." }, // HPDF_PAGE_CANNOT_SET_PARENT
                { "104e", "The current font is not set." }, // HPDF_PAGE_FONT_NOT_FOUND
                { "104f", "An invalid font handle was specified." }, // HPDF_PAGE_INVALID_FONT
                { "1050", "An invalid font size was set." }, // HPDF_PAGE_INVALID_FONT_SIZE
                { "1051", "Invalid graphics mode." }, // HPDF_PAGE_INVALID_GMODE
                { "1052", "Internal error. The consistency of the data was lost." }, // HPDF_PAGE_INVALID_INDEX
                { "1053", "The specified rotate value is not a multiple of 90." }, // HPDF_PAGE_INVALID_ROTATE_VALUE
                { "1054", "An invalid page size was set." }, // HPDF_PAGE_INVALID_SIZE
                { "1055", "An invalid image handle was set." }, // HPDF_PAGE_INVALID_XOBJECT
                { "1056", "The specified value is out of range." }, // HPDF_PAGE_OUT_OF_RANGE
                { "1057", "The specified real value is out of range." }, // HPDF_REAL_OUT_OF_RANGE
                { "1058", "Unexpected EOF marker was detected." }, // HPDF_STREAM_EOF
                { "1059", "Internal error. The consistency of the data was lost." }, // HPDF_STREAM_READLN_CONTINUE
                { "105a", "Reserved/unknown HPDF error." },
                { "105b", "The length of the specified text is too long." }, // HPDF_STRING_OUT_OF_RANGE
                { "105c", "The execution of a function was skipped because of other errors." }, // HPDF_THIS_FUNC_WAS_SKIPPED
                { "105d", "This TrueType font cannot be embedded (restricted by license)." }, // HPDF_TTF_CANNOT_EMBEDDING_FONT
                { "105e", "Unsupported TTF format (invalid cmap)." }, // HPDF_TTF_INVALID_CMAP
                { "105f", "Unsupported TTF format." }, // HPDF_TTF_INVALID_FOMAT
                { "1060", "Unsupported TTF format (missing table)." }, // HPDF_TTF_MISSING_TABLE
                { "1061", "Internal error. The consistency of the data was lost." }, // HPDF_UNSUPPORTED_FONT_TYPE
                { "1062", "Library not configured to use libpng or internal error." }, // HPDF_UNSUPPORTED_FUNC
                { "1063", "Unsupported JPEG format." }, // HPDF_UNSUPPORTED_JPEG_FORMAT
                { "1064", "Failed to parse .PFB file." }, // HPDF_UNSUPPORTED_TYPE1_FONT
                { "1065", "Internal error. The consistency of the data was lost." }, // HPDF_XREF_COUNT_ERR
                { "1066", "An error occurred while executing a function of zlib." }, // HPDF_ZLIB_ERROR
                { "1067", "An error returned from zlib." }, // HPDF_INVALID_PAGE_INDEX
                { "1068", "An invalid URI was set." }, // HPDF_INVALID_URI
                { "1069", "An invalid page layout was set." }, // HPDF_PAGELAYOUT_OUT_OF_RANGE
                { "1070", "An invalid page mode was set." }, // HPDF_PAGEMODE_OUT_OF_RANGE
                { "1071", "An invalid page number style was set." }, // HPDF_PAGENUM_STYLE_OUT_OF_RANGE
                { "1072", "An invalid annotation icon was set." }, // HPDF_ANNOT_INVALID_ICON
                { "1073", "An invalid annotation border style was set." }, // HPDF_ANNOT_INVALID_BORDER_STYLE
                { "1074", "An invalid page direction was set." }, // HPDF_PAGE_INVALID_DIRECTION
                { "1075", "An invalid font handle was specified." } // HPDF_INVALID_FONT
            };
        }
        static const HPDFErrorMap& instance() {
            static HPDFErrorMap inst;
            return inst;
        }
    };

    void error_handler(HPDF_STATUS error_no, HPDF_STATUS, void *) {
        std::ostringstream ss;
        ss << std::hex << error_no;
        std::string error_no_hex = ss.str();

        const auto &err_map = HPDFErrorMap::instance().map;
        auto it = err_map.find(error_no_hex);

        if (it != err_map.end()) {
            std::cerr << "error: error_no=0x" << error_no_hex << ", message=" << it->second<< std::endl;
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

    void DocraftHaruBackend::create_new_page() {
        HPDF_Page new_page = HPDF_AddPage(pdf_);
        if (!new_page) {
            throw std::runtime_error("Failed to create a new page");
        }
        apply_page_format(new_page);
        pages_.push_back(new_page);
        current_page_number_ = pages_.size() - 1; // Move to the newly created page
    }
    DocraftHaruBackend::DocraftHaruBackend() {
        pdf_ = HPDF_New(error_handler, NULL);
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
            for (auto &p: pages_) {
                // Haru automatically manages page lifetimes, so we don't need to free them individually. Just clear the vector to release references.
                p = nullptr;
            }
            pages_.clear();
        }
    }

    void DocraftHaruBackend::begin_text() const {
        HPDF_Page_BeginText(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::end_text() const {
        HPDF_Page_EndText(pages_[internal_current_page_index()]);
    }
    void DocraftHaruBackend::draw_text(const std::string &text, float x, float y) const {
            HPDF_Page_TextOut(pages_[internal_current_page_index()], x, y, text.c_str());
    }

    void DocraftHaruBackend::set_text_color(float r, float g, float b) const {
        HPDF_Page_SetRGBFill(pages_[internal_current_page_index()], r, g, b);
    }

    float DocraftHaruBackend::measure_text_width(const std::string &text) const {
        return HPDF_Page_TextWidth(pages_[internal_current_page_index()], text.c_str());
    }

    void DocraftHaruBackend::set_stroke_color(float r, float g, float b) const {
        HPDF_Page_SetRGBStroke(pages_[internal_current_page_index()], r, g, b);
    }

    void DocraftHaruBackend::set_line_width(float thickness) const {
        HPDF_Page_SetLineWidth(pages_[internal_current_page_index()], thickness);
    }

    void DocraftHaruBackend::draw_line(float x1, float y1, float x2, float y2) const {
        HPDF_Page_MoveTo(pages_[internal_current_page_index()], x1, y1);
        HPDF_Page_LineTo(pages_[internal_current_page_index()], x2, y2);
        HPDF_Page_Stroke(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::draw_text_matrix(
        const std::string &text,
        float scale_x,
        float skew_x,
        float skew_y,
        float scale_y,
        float translate_x,
        float translate_y) const {
        HPDF_Page_SetTextMatrix(pages_[internal_current_page_index()], scale_x, skew_x, skew_y, scale_y, translate_x, translate_y);
        HPDF_Page_ShowText(pages_[internal_current_page_index()], text.c_str());
    }

    void DocraftHaruBackend::save_state() const {
        HPDF_Page_GSave(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::restore_state() const {
        HPDF_Page_GRestore(pages_[internal_current_page_index()]);//restore the previous graphics state, which was saved by the last call of HPDF_Page_GSave() or HPDF_Page_SaveToStream().
    }

    void DocraftHaruBackend::set_fill_color(float r, float g, float b) const {
        HPDF_Page_SetRGBFill(pages_[internal_current_page_index()], r, g, b);
    }

    void DocraftHaruBackend::set_fill_alpha(float alpha) const {
        fill_alpha_ = alpha;
        apply_alpha_state();
    }

    void DocraftHaruBackend::set_stroke_alpha(float alpha) const {
        stroke_alpha_ = alpha;
        apply_alpha_state();
    }

    void DocraftHaruBackend::draw_rectangle(float x, float y, float width, float height) const {
        HPDF_Page_Rectangle(pages_[internal_current_page_index()], x, y, width, height);
    }

    void DocraftHaruBackend::draw_circle(float center_x, float center_y, float radius) const {
        HPDF_Page_Circle(pages_[internal_current_page_index()], center_x, center_y, radius);
    }

    void DocraftHaruBackend::draw_polygon(const std::vector<model::DocraftPoint> &points) const {
        if (points.size() < 2U) {
            return;
        }

        HPDF_Page_MoveTo(pages_[internal_current_page_index()], points[0].x, points[0].y);
        for (size_t i = 1; i < points.size(); ++i) {
            HPDF_Page_LineTo(pages_[internal_current_page_index()], points[i].x, points[i].y);
        }
        HPDF_Page_ClosePath(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::fill() const {
        HPDF_Page_Fill(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::stroke() const {
        HPDF_Page_Stroke(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::fill_stroke() const {
        HPDF_Page_FillStroke(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::draw_png_image(
        const std::string& path,
        float x,
        float y,
        float width,
        float height) const {
        auto image = HPDF_LoadPngImageFromFile(pdf_, path.c_str());
        if (!image) {
            throw std::runtime_error("Failed to load PNG image: " + path);
        }
        HPDF_Page_DrawImage(pages_[internal_current_page_index()], image, x, y, width, height);
    }

    void DocraftHaruBackend::draw_png_image_from_memory(
        const unsigned char* data,
        std::size_t size,
        float x,
        float y,
        float width,
        float height) const {
        auto image = HPDF_LoadPngImageFromMem(
            pdf_,
            reinterpret_cast<const HPDF_BYTE*>(data),
            static_cast<HPDF_UINT>(size));
        if (!image) {
            throw std::runtime_error("Failed to load PNG image from memory");
        }
        HPDF_Page_DrawImage(pages_[internal_current_page_index()], image, x, y, width, height);
    }

    void DocraftHaruBackend::draw_jpeg_image(
        const std::string& path,
        float x,
        float y,
        float width,
        float height) const {
        auto image = HPDF_LoadJpegImageFromFile(pdf_, path.c_str());
        if (!image) {
            throw std::runtime_error("Failed to load JPEG image: " + path);
        }
        HPDF_Page_DrawImage(pages_[internal_current_page_index()], image, x, y, width, height);
    }

    void DocraftHaruBackend::draw_jpeg_image_from_memory(
        const unsigned char* data,
        std::size_t size,
        float x,
        float y,
        float width,
        float height) const {
        auto image = HPDF_LoadJpegImageFromMem(
            pdf_,
            reinterpret_cast<const HPDF_BYTE*>(data),
            static_cast<HPDF_UINT>(size));
        if (!image) {
            throw std::runtime_error("Failed to load JPEG image from memory");
        }
        HPDF_Page_DrawImage(pages_[internal_current_page_index()], image, x, y, width, height);
    }

    void DocraftHaruBackend::draw_raw_rgb_image(
        const std::string& path,
        int pixel_width,
        int pixel_height,
        float x,
        float y,
        float width,
        float height) const {
        auto image = HPDF_LoadRawImageFromFile(
            pdf_,
            path.c_str(),
            static_cast<HPDF_UINT>(pixel_width),
            static_cast<HPDF_UINT>(pixel_height),
            HPDF_CS_DEVICE_RGB);
        if (!image) {
            throw std::runtime_error("Failed to load raw RGB image: " + path);
        }
        HPDF_Page_DrawImage(pages_[internal_current_page_index()], image, x, y, width, height);
    }

    void DocraftHaruBackend::draw_raw_rgb_image_from_memory(
        const unsigned char* data,
        int pixel_width,
        int pixel_height,
        float x,
        float y,
        float width,
        float height) const {
        constexpr HPDF_UINT bits_per_component = 8;
        auto *image = HPDF_LoadRawImageFromMem(
            pdf_,
            reinterpret_cast<const HPDF_BYTE*>(data),
            static_cast<HPDF_UINT>(pixel_width),
            static_cast<HPDF_UINT>(pixel_height),
            HPDF_CS_DEVICE_RGB,
            bits_per_component);
        if (!image) {
            throw std::runtime_error("Failed to load raw RGB image from memory");
        }
        HPDF_Page_DrawImage(pages_[internal_current_page_index()], image, x, y, width, height);
    }

    float DocraftHaruBackend::page_width() const {
        return HPDF_Page_GetWidth(pages_[internal_current_page_index()]);
    }

    float DocraftHaruBackend::page_height() const {
        return HPDF_Page_GetHeight(pages_[internal_current_page_index()]);
    }

    void DocraftHaruBackend::save_to_file(const std::string& path) const {
        HPDF_SaveToFile(pdf_, path.c_str());
    }

    std::string DocraftHaruBackend::file_extension() const {
        return ".pdf";
    }

    const char* DocraftHaruBackend::register_ttf_font_from_file(
        const std::string& path,
        bool embed) const {
        const char* result = HPDF_LoadTTFontFromFile(pdf_, path.c_str(), embed ? HPDF_TRUE : HPDF_FALSE);
        if (!result) {
            HPDF_ResetError(pdf_);
        }
        return result;
    }

    bool DocraftHaruBackend::can_use_font(
        const std::string& internal_name,
        const char* encoder) const {
        HPDF_Font font = HPDF_GetFont(pdf_, internal_name.c_str(), encoder);
        if (!font || HPDF_GetError(pdf_) != HPDF_OK) {
            HPDF_ResetError(pdf_);
            return false;
        }
        return true;
    }

    void DocraftHaruBackend::set_font(
        const std::string& internal_name,
        float size,
        const char* encoder) const {
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

    void DocraftHaruBackend::add_new_page() {
        create_new_page();
    }

    void DocraftHaruBackend::move_to_next_page() {
            if (current_page_number_ + 1 < pages_.size()) {
                ++current_page_number_;
            } else {
                throw std::runtime_error("Already at the last page, cannot move to next page");
            }
    }

    void DocraftHaruBackend::go_to_page(std::size_t page_number) {
        if (page_number < pages_.size()) {
            current_page_number_ = page_number;
        } else {
            throw std::runtime_error("Invalid page number: " + std::to_string(page_number));
        }
    }

    void DocraftHaruBackend::go_to_first_page() {
        if (pages_.empty()) {
            throw std::runtime_error("No pages in document");
        }
        current_page_number_ = 0;
    }

    void DocraftHaruBackend::go_to_previous_page() {
        if (current_page_number_ == 0) {
            throw std::runtime_error("Already at the first page, cannot move to previous page");
        }
        --current_page_number_;
    }

    void DocraftHaruBackend::go_to_last_page() {
        if (pages_.empty()) {
            throw std::runtime_error("No pages in document");
        }
        current_page_number_ = pages_.size() - 1;
    }

    std::size_t DocraftHaruBackend::current_page_number() const {
        return current_page_number_+1; // Return 1-based page number for external use
    }

    std::size_t DocraftHaruBackend::total_page_count() const {
        return pages_.size();
    }
    size_t DocraftHaruBackend::internal_current_page_index() const {
        return current_page_number_;
    }

    void DocraftHaruBackend::apply_page_format(HPDF_Page page) const {
        HPDF_Page_SetSize(page, page_size_, page_direction_);
    }

    void DocraftHaruBackend::set_page_format(model::DocraftPageSize size,
                                             model::DocraftPageOrientation orientation) {
        page_size_ = to_hpdf_size(size);
        page_direction_ = to_hpdf_direction(orientation);
        for (auto &page : pages_) {
            if (page) {
                apply_page_format(page);
            }
        }
    }

} // docraft

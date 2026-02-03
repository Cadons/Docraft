#include "docraft_document_context.h"
#include <hpdf.h>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "renderer/docraft_pdf_renderer.h"
#pragma region HARU Error Handling
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

static void error_handler(HPDF_STATUS error_no, HPDF_STATUS , void *) {
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
#pragma endregion

namespace docraft {
    DocraftDocumentContext::DocraftDocumentContext() {
        pdf_doc_ = HPDF_New(error_handler,NULL);
        HPDF_UseUTFEncodings(pdf_doc_);
        HPDF_SetCurrentEncoder(pdf_doc_, "UTF-8");
        HPDF_SetCompressionMode(pdf_doc_, HPDF_COMP_ALL);
        page_ = HPDF_AddPage(pdf_doc_);
        float page_height = HPDF_Page_GetHeight(page_);
        page_height_ = page_height;
        page_width_ = HPDF_Page_GetWidth(page_);

        current_rect_width_ = page_width_;
    }

    DocraftDocumentContext::~DocraftDocumentContext() = default;
#pragma region setter
    void DocraftDocumentContext::set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer) {
        renderer_ = renderer;
    }

    void DocraftDocumentContext::set_current_rect_width(float current_rect_width) {
        current_rect_width_ = current_rect_width;
    }
    void DocraftDocumentContext::set_header(const std::shared_ptr<model::DocraftHeader> &header) {
        header_ = header;
    }
    void DocraftDocumentContext::set_body(const std::shared_ptr<model::DocraftBody> &body) {
        body_ = body;
    }
    void DocraftDocumentContext::set_footer(const std::shared_ptr<model::DocraftFooter> &footer) {
        footer_ = footer;
    }
    void DocraftDocumentContext::set_font_applier(const std::shared_ptr<generic::DocraftFontApplier> &font_applier) {
        font_applier_ = font_applier;
    }
#pragma endregion
#pragma region getter
    HPDF_Doc DocraftDocumentContext::pdf_doc() const {
        return pdf_doc_;
    }

    HPDF_Page DocraftDocumentContext::page() const {
        return page_;
    }

    DocraftCursor &DocraftDocumentContext::cursor() {
        return cursor_;
    }
    float DocraftDocumentContext::available_space() const {
        return current_rect_width_ ;
    }
    std::shared_ptr<renderer::DocraftAbstractRenderer> DocraftDocumentContext::renderer() {
        if (!renderer_) {
            throw std::runtime_error("Renderer not set in DocraftPDFContext");
        }
        return renderer_;
    }

    float DocraftDocumentContext::page_height() const {
        return page_height_;
    }
    float DocraftDocumentContext::page_width() const {
        return page_width_;
    }


    const std::shared_ptr<model::DocraftHeader>& DocraftDocumentContext::header() const {
        return header_;
    }


    const std::shared_ptr<model::DocraftBody>& DocraftDocumentContext::body() const {
        return body_;
    }

    const std::shared_ptr<model::DocraftFooter>& DocraftDocumentContext::footer() const {
        return footer_;
    }
    const std::shared_ptr<generic::DocraftFontApplier>& DocraftDocumentContext::font_applier() const {
        return font_applier_;
    }
#pragma endregion
} // docraft

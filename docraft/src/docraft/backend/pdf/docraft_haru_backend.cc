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
#include "docraft/backend/pdf/docraft_haru_font_backend.h"
#include "docraft/backend/pdf/docraft_haru_image_backend.h"
#include "docraft/backend/pdf/docraft_haru_line_backend.h"
#include "docraft/backend/pdf/docraft_haru_metadata_backend.h"
#include "docraft/backend/pdf/docraft_haru_output_backend.h"
#include "docraft/backend/pdf/docraft_haru_page_backend.h"
#include "docraft/backend/pdf/docraft_haru_shape_backend.h"
#include "docraft/backend/pdf/docraft_haru_text_backend.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <hpdf.h>

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
        HPDF_Doc create_hpdf_document() {
            HPDF_Doc pdf = HPDF_New(error_handler, nullptr);
            if (!pdf) {
                throw std::runtime_error("Failed to initialize Haru PDF document");
            }
            return pdf;
        }
    } // namespace

    DocraftHaruBackend::DocraftHaruBackend()
        : state_(std::make_shared<DocraftHaruSharedState>()) {
        state_->pdf = create_hpdf_document();
        output_backend_ = std::make_unique<DocraftHaruOutputBackend>(state_);
        page_backend_ = std::make_unique<DocraftHaruPageBackend>(state_);
        font_backend_ = std::make_unique<DocraftHaruFontBackend>(state_);
        metadata_backend_ = std::make_unique<DocraftHaruMetadataBackend>(state_);
        text_backend_ = std::make_unique<DocraftHaruTextBackend>(state_);
        line_backend_ = std::make_unique<DocraftHaruLineBackend>(state_);
        shape_backend_ = std::make_unique<DocraftHaruShapeBackend>(state_);
        image_backend_ = std::make_unique<DocraftHaruImageBackend>(state_);

        HPDF_UseUTFEncodings(state_->pdf);
        HPDF_SetCurrentEncoder(state_->pdf, "UTF-8");
        HPDF_SetCompressionMode(state_->pdf, HPDF_COMP_ALL);
        page_backend_->add_new_page();
    }

    DocraftHaruBackend::~DocraftHaruBackend() {
        // Destroy capability backends explicitly to enforce teardown order.
        image_backend_.reset();
        shape_backend_.reset();
        line_backend_.reset();
        text_backend_.reset();
        page_backend_.reset();
        metadata_backend_.reset();
        font_backend_.reset();
        output_backend_.reset();

        if (state_ && state_->pdf) {
            HPDF_Free(state_->pdf);
            state_->pdf = nullptr;
        }
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

    const docraft::backend::IDocraftOutputBackend *DocraftHaruBackend::output_backend() const {
        return output_backend_.get();
    }

    docraft::backend::IDocraftOutputBackend *DocraftHaruBackend::edit_output_backend() {
        return output_backend_.get();
    }

    const docraft::backend::IDocraftFontBackend *DocraftHaruBackend::font_backend() const {
        return font_backend_.get();
    }

    docraft::backend::IDocraftFontBackend *DocraftHaruBackend::edit_font_backend() {
        return font_backend_.get();
    }

    const docraft::backend::IDocraftMetadataBackend *DocraftHaruBackend::metadata_backend() const {
        return metadata_backend_.get();
    }

    docraft::backend::IDocraftMetadataBackend *DocraftHaruBackend::edit_metadata_backend() {
        return metadata_backend_.get();
    }

    HPDF_Doc DocraftHaruBackend::pdf_document() const {
        return state_ ? state_->pdf : nullptr;
    }
} // docraft::backend::pdf

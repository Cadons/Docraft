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

#include "docraft/backend/pdf/docraft_haru_metadata_backend.h"

#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#include <hpdf.h>

#include "docraft/docraft_document_metadata.h"

namespace docraft::backend::pdf {
    namespace {
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

    DocraftHaruMetadataBackend::DocraftHaruMetadataBackend(const std::shared_ptr<DocraftHaruSharedState> &state)
        : state_(state) {
    }

    void DocraftHaruMetadataBackend::set_document_metadata(const DocraftDocumentMetadata &metadata) {
        const auto pdf = state_ ? state_->pdf : nullptr;
        if (!pdf) {
            throw std::runtime_error("Haru document is not initialized");
        }
        set_info_date_attr_if_present(pdf, HPDF_INFO_CREATION_DATE, metadata.creation_date(), "creation_date");
        set_info_date_attr_if_present(pdf, HPDF_INFO_MOD_DATE, metadata.modification_date(),
                                      "modification_date");
        set_info_attr_if_present(pdf, HPDF_INFO_AUTHOR, metadata.author(), "author");
        set_info_attr_if_present(pdf, HPDF_INFO_CREATOR, metadata.creator(), "creator");
        set_info_attr_if_present(pdf, HPDF_INFO_PRODUCER, metadata.producer(), "producer");
        set_info_attr_if_present(pdf, HPDF_INFO_TITLE, metadata.title(), "title");
        set_info_attr_if_present(pdf, HPDF_INFO_SUBJECT, metadata.subject(), "subject");
        set_info_attr_if_present(pdf, HPDF_INFO_KEYWORDS, metadata.keywords(), "keywords");
        set_info_attr_if_present(pdf, HPDF_INFO_TRAPPED, metadata.trapped(), "trapped");
        set_info_attr_if_present(pdf, HPDF_INFO_GTS_PDFX, metadata.gts_pdfx(), "gts_pdfx");
    }
} // namespace docraft::backend::pdf

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

#include "docraft/management/docraft_document_section_manager.h"
#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_footer.h"

namespace docraft::management {
    void DocraftDocumentSectionManager::set_header(const std::shared_ptr<model::DocraftHeader> &header) {
        header_ = header;
    }

    std::shared_ptr<const model::DocraftHeader> DocraftDocumentSectionManager::header() const {
        return header_;
    }

    std::shared_ptr<model::DocraftHeader> DocraftDocumentSectionManager::edit_header() {
        return header_;
    }

    void DocraftDocumentSectionManager::set_body(const std::shared_ptr<model::DocraftBody> &body) {
        body_ = body;
    }

    std::shared_ptr<const model::DocraftBody> DocraftDocumentSectionManager::body() const {
        return body_;
    }

    std::shared_ptr<model::DocraftBody> DocraftDocumentSectionManager::edit_body() {
        return body_;
    }

    void DocraftDocumentSectionManager::set_footer(const std::shared_ptr<model::DocraftFooter> &footer) {
        footer_ = footer;
    }

    std::shared_ptr<const model::DocraftFooter> DocraftDocumentSectionManager::footer() const {
        return footer_;
    }

    std::shared_ptr<model::DocraftFooter> DocraftDocumentSectionManager::edit_footer() {
        return footer_;
    }

    void DocraftDocumentSectionManager::set_section_ratios(float header_ratio, float body_ratio, float footer_ratio) {
        header_ratio_ = header_ratio;
        body_ratio_ = body_ratio;
        footer_ratio_ = footer_ratio;
    }

    float DocraftDocumentSectionManager::header_ratio() const { return header_ratio_; }
    float DocraftDocumentSectionManager::body_ratio() const { return body_ratio_; }
    float DocraftDocumentSectionManager::footer_ratio() const { return footer_ratio_; }
} // docraft::management


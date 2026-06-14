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

#include "docraft/services/docraft_navigation_service.h"
#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_footer.h"

namespace docraft::services {
    NavigationService::NavigationService()
        : section_manager_(std::make_unique<management::DocraftDocumentSectionManager>()) {
    }

    NavigationService::~NavigationService() = default;

    management::DocraftDocumentSectionManager &NavigationService::section_manager() {
        return *section_manager_;
    }

    const management::DocraftDocumentSectionManager &NavigationService::section_manager() const {
        return *section_manager_;
    }

    void NavigationService::set_header(const std::shared_ptr<model::DocraftHeader> &header) {
        section_manager_->set_header(header);
    }

    std::shared_ptr<const model::DocraftHeader> NavigationService::header() const {
        return section_manager_->header();
    }

    std::shared_ptr<model::DocraftHeader> NavigationService::edit_header() {
        return section_manager_->edit_header();
    }

    void NavigationService::set_body(const std::shared_ptr<model::DocraftBody> &body) {
        section_manager_->set_body(body);
    }

    std::shared_ptr<const model::DocraftBody> NavigationService::body() const {
        return section_manager_->body();
    }

    std::shared_ptr<model::DocraftBody> NavigationService::edit_body() {
        return section_manager_->edit_body();
    }

    void NavigationService::set_footer(const std::shared_ptr<model::DocraftFooter> &footer) {
        section_manager_->set_footer(footer);
    }

    std::shared_ptr<const model::DocraftFooter> NavigationService::footer() const {
        return section_manager_->footer();
    }

    std::shared_ptr<model::DocraftFooter> NavigationService::edit_footer() {
        return section_manager_->edit_footer();
    }

    void NavigationService::set_section_ratios(float header_ratio, float body_ratio, float footer_ratio) {
        section_manager_->set_section_ratios(header_ratio, body_ratio, footer_ratio);
    }

    float NavigationService::header_ratio() const {
        return section_manager_->header_ratio();
    }

    float NavigationService::body_ratio() const {
        return section_manager_->body_ratio();
    }

    float NavigationService::footer_ratio() const {
        return section_manager_->footer_ratio();
    }
} // namespace docraft::services




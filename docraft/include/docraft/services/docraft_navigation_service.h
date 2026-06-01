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

#pragma once

#include "docraft/docraft_lib.h"
#include "docraft/management/docraft_document_section_manager.h"
#include <memory>

namespace docraft::model {
    class DocraftHeader;
    class DocraftBody;
    class DocraftFooter;
}

namespace docraft::services {
    /**
     * @brief Manages document structure: header, body, footer, and page navigation.
     *
     * Responsible for:
     * - Managing document sections (header, body, footer)
     * - Handling section ratios
     * - Navigating pages
     */
    class DOCRAFT_LIB NavigationService {
    public:
        NavigationService();

        ~NavigationService();

        /**
         * @brief Returns the section manager.
         */
        management::DocraftDocumentSectionManager &section_manager();

        [[nodiscard]] const management::DocraftDocumentSectionManager &section_manager() const;

        /**
         * @brief Sets the header section.
         */
        void set_header(const std::shared_ptr<model::DocraftHeader> &header);

        /**
         * @brief Returns the header.
         */
        [[nodiscard]] std::shared_ptr<const model::DocraftHeader> header() const;

        [[nodiscard]] std::shared_ptr<model::DocraftHeader> edit_header();

        /**
         * @brief Sets the body section.
         */
        void set_body(const std::shared_ptr<model::DocraftBody> &body);

        /**
         * @brief Returns the body.
         */
        [[nodiscard]] std::shared_ptr<const model::DocraftBody> body() const;

        [[nodiscard]] std::shared_ptr<model::DocraftBody> edit_body();

        /**
         * @brief Sets the footer section.
         */
        void set_footer(const std::shared_ptr<model::DocraftFooter> &footer);

        /**
         * @brief Returns the footer.
         */
        [[nodiscard]] std::shared_ptr<const model::DocraftFooter> footer() const;

        [[nodiscard]] std::shared_ptr<model::DocraftFooter> edit_footer();

        /**
         * @brief Sets section height ratios (header:body:footer).
         */
        void set_section_ratios(float header_ratio, float body_ratio, float footer_ratio);

        [[nodiscard]] float header_ratio() const;

        [[nodiscard]] float body_ratio() const;

        [[nodiscard]] float footer_ratio() const;

    private:
        std::unique_ptr<management::DocraftDocumentSectionManager> section_manager_;
    };
} // namespace docraft::services


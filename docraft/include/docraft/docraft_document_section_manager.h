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
#include <memory>

namespace docraft::model {
    class DocraftHeader;
    class DocraftBody;
    class DocraftFooter;
}

namespace docraft {
    /**
     * @brief Manages document sections (header, body, footer) and their ratios.
     */
    class DOCRAFT_LIB DocraftDocumentSectionManager {
    public:
        /**
         * @brief Sets the document header node.
         * @param header Header node.
         */
        void set_header(const std::shared_ptr<model::DocraftHeader> &header);

        /**
         * @brief Returns the header node.
         * @return Header node (may be nullptr).
         */
        [[nodiscard]] std::shared_ptr<const model::DocraftHeader> header() const;

        [[nodiscard]] std::shared_ptr<model::DocraftHeader> edit_header();

        /**
         * @brief Sets the document body node.
         * @param body Body node.
         */
        void set_body(const std::shared_ptr<model::DocraftBody> &body);

        /**
         * @brief Returns the body node.
         * @return Body node (may be nullptr).
         */
        [[nodiscard]] std::shared_ptr<const model::DocraftBody> body() const;

        [[nodiscard]] std::shared_ptr<model::DocraftBody> edit_body();

        /**
         * @brief Sets the document footer node.
         * @param footer Footer node.
         */
        void set_footer(const std::shared_ptr<model::DocraftFooter> &footer);

        /**
         * @brief Returns the footer node.
         * @return Footer node (may be nullptr).
         */
        [[nodiscard]] std::shared_ptr<const model::DocraftFooter> footer() const;

        [[nodiscard]] std::shared_ptr<model::DocraftFooter> edit_footer();

        /**
         * @brief Sets header/body/footer ratios.
         */
        void set_section_ratios(float header_ratio, float body_ratio, float footer_ratio);

        /**
         * @brief Returns the header ratio.
         */
        [[nodiscard]] float header_ratio() const;

        /**
         * @brief Returns the body ratio.
         */
        [[nodiscard]] float body_ratio() const;

        /**
         * @brief Returns the footer ratio.
         */
        [[nodiscard]] float footer_ratio() const;

    private:
        std::shared_ptr<model::DocraftHeader> header_;
        std::shared_ptr<model::DocraftBody> body_;
        std::shared_ptr<model::DocraftFooter> footer_;
        float header_ratio_ = 0.06F;
        float body_ratio_ = 0.88F;
        float footer_ratio_ = 0.06F;
    };
}


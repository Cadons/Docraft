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

#include <optional>
#include <string>

namespace docraft {
    /**
     * @brief Metadata container for PDF document information.
     *
     * Supports all document info fields exposed by libharu
     * (string attributes plus creation and modification dates).
     */
    class DOCRAFT_LIB DocraftDocumentMetadata {
    public:
        /**
         * @brief Date/time structure compatible with libharu info dates.
         */
        struct DateTime {
            int year = 0;
            int month = 0;
            int day = 0;
            int hour = 0;
            int minutes = 0;
            int seconds = 0;
            char ind = '+';
            int off_hour = 0;
            int off_minutes = 0;
        };

        void set_author(const std::string &author);
        void set_creator(const std::string &creator);
        void set_producer(const std::string &producer);
        void set_title(const std::string &title);
        void set_subject(const std::string &subject);
        void set_keywords(const std::string &keywords);
        void set_trapped(const std::string &trapped);
        void set_gts_pdfx(const std::string &gts_pdfx);
        void set_creation_date(const DateTime &creation_date);
        void set_modification_date(const DateTime &modification_date);

        void clear_author();
        void clear_creator();
        void clear_producer();
        void clear_title();
        void clear_subject();
        void clear_keywords();
        void clear_trapped();
        void clear_gts_pdfx();
        void clear_creation_date();
        void clear_modification_date();

        [[nodiscard]] const std::optional<std::string> &author() const;
        [[nodiscard]] const std::optional<std::string> &creator() const;
        [[nodiscard]] const std::optional<std::string> &producer() const;
        [[nodiscard]] const std::optional<std::string> &title() const;
        [[nodiscard]] const std::optional<std::string> &subject() const;
        [[nodiscard]] const std::optional<std::string> &keywords() const;
        [[nodiscard]] const std::optional<std::string> &trapped() const;
        [[nodiscard]] const std::optional<std::string> &gts_pdfx() const;
        [[nodiscard]] const std::optional<DateTime> &creation_date() const;
        [[nodiscard]] const std::optional<DateTime> &modification_date() const;

    private:
        std::optional<std::string> author_;
        std::optional<std::string> creator_;
        std::optional<std::string> producer_;
        std::optional<std::string> title_;
        std::optional<std::string> subject_;
        std::optional<std::string> keywords_;
        std::optional<std::string> trapped_;
        std::optional<std::string> gts_pdfx_;
        std::optional<DateTime> creation_date_;
        std::optional<DateTime> modification_date_;
    };
} // namespace docraft

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

        void set_author(const std::string &author) { author_ = author; }
        void set_creator(const std::string &creator) { creator_ = creator; }
        void set_producer(const std::string &producer) { producer_ = producer; }
        void set_title(const std::string &title) { title_ = title; }
        void set_subject(const std::string &subject) { subject_ = subject; }
        void set_keywords(const std::string &keywords) { keywords_ = keywords; }
        void set_trapped(const std::string &trapped) { trapped_ = trapped; }
        void set_gts_pdfx(const std::string &gts_pdfx) { gts_pdfx_ = gts_pdfx; }
        void set_creation_date(const DateTime &creation_date) { creation_date_ = creation_date; }
        void set_modification_date(const DateTime &modification_date) { modification_date_ = modification_date; }

        void clear_author() { author_.reset(); }
        void clear_creator() { creator_.reset(); }
        void clear_producer() { producer_.reset(); }
        void clear_title() { title_.reset(); }
        void clear_subject() { subject_.reset(); }
        void clear_keywords() { keywords_.reset(); }
        void clear_trapped() { trapped_.reset(); }
        void clear_gts_pdfx() { gts_pdfx_.reset(); }
        void clear_creation_date() { creation_date_.reset(); }
        void clear_modification_date() { modification_date_.reset(); }

        [[nodiscard]] const std::optional<std::string> &author() const { return author_; }
        [[nodiscard]] const std::optional<std::string> &creator() const { return creator_; }
        [[nodiscard]] const std::optional<std::string> &producer() const { return producer_; }
        [[nodiscard]] const std::optional<std::string> &title() const { return title_; }
        [[nodiscard]] const std::optional<std::string> &subject() const { return subject_; }
        [[nodiscard]] const std::optional<std::string> &keywords() const { return keywords_; }
        [[nodiscard]] const std::optional<std::string> &trapped() const { return trapped_; }
        [[nodiscard]] const std::optional<std::string> &gts_pdfx() const { return gts_pdfx_; }
        [[nodiscard]] const std::optional<DateTime> &creation_date() const { return creation_date_; }
        [[nodiscard]] const std::optional<DateTime> &modification_date() const { return modification_date_; }

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

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

#include "docraft/backend/pdf/docraft_haru_backend.h"

#include <vector>

namespace docraft::backend::pdf {
    /**
     * @brief Haru implementation of page management operations.
     */
    class DocraftHaruBackend::PageHaruBackend : public docraft::backend::IDocraftPageRenderingBackend {
    public:
        /**
         * @brief Creates a page backend bound to a Haru document owner.
         */
        explicit PageHaruBackend(DocraftHaruBackend &owner);

        /**
         * @brief Returns the width of the current page.
         */
        float page_width() const override;

        /**
         * @brief Returns the height of the current page.
         */
        float page_height() const override;

        /**
         * @brief Appends a new page and moves the cursor to it.
         */
        void add_new_page() override;

        /**
         * @brief Moves to the next page if available.
         */
        void move_to_next_page() override;

        /**
         * @brief Moves to the page identified by zero-based index.
         */
        void go_to_page(std::size_t page_number) override;

        /**
         * @brief Moves to the first page.
         */
        void go_to_first_page() override;

        /**
         * @brief Moves to the previous page.
         */
        void go_to_previous_page() override;

        /**
         * @brief Moves to the last page.
         */
        void go_to_last_page() override;

        /**
         * @brief Applies the page size and orientation to all existing pages.
         */
        void set_page_format(model::DocraftPageSize size,
                             model::DocraftPageOrientation orientation) override;

        /**
         * @brief Returns the current page number (1-based).
         */
        std::size_t current_page_number() const override;

        /**
         * @brief Returns the total number of pages in the document.
         */
        std::size_t total_page_count() const override;

        /**
         * @brief Returns the current page handle for internal capability backends.
         */
        [[nodiscard]] HPDF_Page current_page() const;

        /**
         * @brief Returns the current page index (0-based) for internal capability backends.
         */
        [[nodiscard]] std::size_t current_page_index() const;

    private:
        void apply_page_format(HPDF_Page page) const;

        DocraftHaruBackend &owner_;
        std::vector<HPDF_Page> pages_;
        std::size_t current_page_number_ = 0;
        HPDF_PageSizes page_size_ = HPDF_PAGE_SIZE_A4;
        HPDF_PageDirection page_direction_ = HPDF_PAGE_PORTRAIT;
    };
} // namespace docraft::backend::pdf


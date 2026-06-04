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

#include "docraft/backend/docraft_page_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"

#include <memory>
#include <vector>

namespace docraft::backend::pdf {
    /**
     * @brief Haru implementation of page management operations.
     *
     * Implements both IDocraftPageRenderingBackend (for public page operations)
     * and IPageOperationsProvider (for internal capability backend access).
     *
     * Page state is centralized in DocraftHaruSharedState (pages_, current_page_index_,
     * page_size_, page_direction_) to avoid state distribution.
     *
     * LIFETIME: Must register itself as the page operations provider with the
     * shared state immediately upon construction, and be destroyed only after
     * all capability backends that depend on it are destroyed.
     */
    class DocraftHaruPageBackend : public docraft::backend::IDocraftPageRenderingBackend,
                                   public IPageOperationsProvider {
    public:
        /**
         * @brief Creates a page backend bound to a Haru document owner.
         *
         * Automatically registers itself as the page operations provider.
         * Uses state's centralized page_state for all page management.
         */
        explicit DocraftHaruPageBackend(const std::shared_ptr<DocraftHaruSharedState> &state);

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
        [[nodiscard]] HPDF_Page current_page() const override;

        /**
         * @brief Returns the current page index (0-based) for internal capability backends.
         */
        [[nodiscard]] std::size_t current_page_index() const override;

    private:
        void apply_page_format(HPDF_Page page) const;

        std::shared_ptr<DocraftHaruSharedState> state_;
    };
} // namespace docraft::backend::pdf


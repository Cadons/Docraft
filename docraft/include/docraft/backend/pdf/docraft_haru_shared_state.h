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

#include <hpdf.h>
#include <vector>
#include <cstddef>

#include "docraft/exception/docraft_exceptions.h"

namespace docraft::backend::pdf {
    /**
     * @brief Observer interface for page operations.
     *
     * This interface allows capability backends to access page operations without
     * maintaining direct raw pointers to DocraftHaruPageBackend. The lifetime contract
     * is: the provider MUST remain valid for the entire lifetime of the shared state
     * that holds it. This is guaranteed by keeping the provider registration and
     * shared state lifecycle in sync within DocraftHaruBackend.
     */
    class IPageOperationsProvider {
    public:
        virtual ~IPageOperationsProvider() = default;

        /**
         * @brief Returns the current page handle.
         * @note Must not be nullptr while provider is registered.
         */
        virtual HPDF_Page current_page() const = 0;

        /**
         * @brief Returns the width of the current page in points.
         */
        virtual float page_width() const = 0;

        /**
         * @brief Returns the height of the current page in points.
         */
        virtual float page_height() const = 0;

        /**
         * @brief Returns the current page index (0-based).
         */
        virtual std::size_t current_page_index() const = 0;
    };

    /**
     * @brief Centralized graphics state (color, alpha).
     *
     * Prevents state distribution across multiple backends by centralizing
     * all graphics state in one place. Shape and other backends query/modify
     * this state through the shared state.
     */
    struct GraphicsState {
        float fill_alpha = 1.0F;
        float stroke_alpha = 1.0F;
        // Future: fill_color, stroke_color can be added here
    };

    /**
     * @brief Centralized page state (pages, current page index, format).
     *
     * Centralizes all page-related state that was previously scattered
     * between DocraftHaruPageBackend and other backends.
     */
    struct PageState {
        std::vector<HPDF_Page> pages;
        std::size_t current_page_index = 0;
        HPDF_PageSizes page_size = HPDF_PAGE_SIZE_A4;
        HPDF_PageDirection page_direction = HPDF_PAGE_PORTRAIT;
    };

    /**
     * @brief Shared Haru document state used by capability backends.
     *
     * LIFETIME & OWNERSHIP CONTRACT:
     * - `pdf`: Owned by this struct (created/destroyed by DocraftHaruBackend)
     * - `page_operations_provider`: Non-owning observer, registered during init
     * - `page_state`: Owned by this struct (all page management centralized)
     * - `graphics_state`: Owned by this struct (all graphics state centralized)
     *
     * INVARIANTS:
     * 1. page_operations_provider MUST be set before any operation uses it
     * 2. page_state must be valid after initialization
     * 3. graphics_state must be valid after initialization
     * 4. All state changes go through this struct's methods for consistency
     */
    struct DocraftHaruSharedState {
        // PDF document owner - lifetime managed by DocraftHaruBackend
        HPDF_Doc pdf = nullptr;

        // Centralized page state (was scattered in DocraftHaruPageBackend)
        PageState page_state;

        // Centralized graphics state (was scattered in DocraftHaruShapeBackend)
        GraphicsState graphics_state;

        // Non-owning observer for page operations
        IPageOperationsProvider *page_operations_provider = nullptr;

        /**
         * @brief Registers the page operations provider.
         *
         * CONTRACT: Must be called exactly once during initialization,
         * before any capability backend accesses page operations.
         */
        void set_page_operations_provider(IPageOperationsProvider *provider) {
            page_operations_provider = provider;
        }

        /**
         * @brief Clears the page operations provider.
         *
         * CONTRACT: Must be called exactly once during destruction,
         * after all capability backends are destroyed.
         */
        void clear_page_operations_provider() {
            page_operations_provider = nullptr;
        }

        /**
         * @brief Ensures the provider is available.
         *
         * @throws docraft::exception::BackendStateException if provider is nullptr.
         */
        IPageOperationsProvider *ensure_page_provider() const {
            if (!page_operations_provider) {
                throw docraft::exception::BackendStateException(
                    "Page operations provider not set. "
                    "Capability backend accessed before initialization."
                );
            }
            return page_operations_provider;
        }

        [[nodiscard]] float fill_alpha() const {
            return graphics_state.fill_alpha;
        }

        [[nodiscard]] float &edit_fill_alpha() {
            return graphics_state.fill_alpha;
        }

        [[nodiscard]] float stroke_alpha() const {
            return graphics_state.stroke_alpha;
        }

        [[nodiscard]] float &edit_stroke_alpha() {
            return graphics_state.stroke_alpha;
        }

        [[nodiscard]] HPDF_PageSizes page_size() const {
            return page_state.page_size;
        }

        [[nodiscard]] HPDF_PageSizes &edit_page_size() {
            return page_state.page_size;
        }

        [[nodiscard]] HPDF_PageDirection page_direction() const {
            return page_state.page_direction;
        }

        [[nodiscard]] HPDF_PageDirection &edit_page_direction() {
            return page_state.page_direction;
        }

        /**
         * @brief Adds a page to the internal pages vector and returns it.
         */
        HPDF_Page add_page(HPDF_Page page) {
            page_state.pages.push_back(page);
            return page;
        }

        /**
         * @brief Returns the total number of pages.
         */
        std::size_t page_count() const {
            return page_state.pages.size();
        }

        /**
         * @brief Returns the current page index (0-based).
         */
        [[nodiscard]] std::size_t current_page_index() const {
            return page_state.current_page_index;
        }

        [[nodiscard]] std::size_t &edit_current_page_index() {
            return page_state.current_page_index;
        }

        [[nodiscard]] std::vector<HPDF_Page> &edit_pages() {
            return page_state.pages;
        }

        [[nodiscard]] const std::vector<HPDF_Page> &pages() const {
            return page_state.pages;
        }
    };
} // namespace docraft::backend::pdf


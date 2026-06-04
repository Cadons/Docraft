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
     * @brief Shared Haru document state used by capability backends.
     *
     * LIFETIME CONTRACT:
     * - `pdf` is owned by this struct (created/destroyed by DocraftHaruBackend)
     * - `page_operations_provider` is a non-owning observer that MUST be set before
     *   any capability backend uses it, and MUST NOT be cleared until all capability
     *   backends are destroyed. The owner (DocraftHaruBackend) is responsible for
     *   maintaining this invariant.
     */
    struct DocraftHaruSharedState {
        HPDF_Doc pdf = nullptr;

        /// Non-owning observer. Valid lifetime guaranteed by owner (DocraftHaruBackend).
        /// Set during initialization, cleared during destruction, never reassigned.
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
         * @throws std::runtime_error if provider is nullptr.
         */
        IPageOperationsProvider *ensure_page_provider() const {
            if (!page_operations_provider) {
                throw std::runtime_error(
                    "Page operations provider not set. "
                    "Capability backend accessed before initialization."
                );
            }
            return page_operations_provider;
        }
    };
} // namespace docraft::backend::pdf


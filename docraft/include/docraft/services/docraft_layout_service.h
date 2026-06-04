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
#include "docraft/docraft_cursor.h"
#include "docraft/model/docraft_page_format.h"
#include <memory>

namespace docraft::services {
    /**
     * @brief Manages document layout state: cursor position and page metrics.
     *
     * Responsible for:
     * - Tracking current cursor position during layout
     * - Storing page width/height
     * - Computing available vertical space
     * - Handling page format changes
     */
    class DOCRAFT_LIB LayoutService {
    public:
        LayoutService();

        ~LayoutService();

        /**
         * @brief Returns the layout cursor.
         */
        DocraftCursor &cursor();

        [[nodiscard]] const DocraftCursor &cursor() const;

        /**
         * @brief Returns page width in points.
         */
        [[nodiscard]] float page_width() const;

        /**
         * @brief Returns page height in points.
         */
        [[nodiscard]] float page_height() const;

        /**
         * @brief Returns remaining vertical space on current section.
         */
        [[nodiscard]] float available_space() const;

        /**
         * @brief Sets the current layout rectangle width.
         */
        void set_current_rect_width(float width);

        [[nodiscard]] float current_rect_width() const;

        /**
         * @brief Overrides page dimensions directly (used when backend exposes concrete page size).
         */
        void set_page_dimensions(float width, float height);

        /**
         * @brief Sets page format and updates cached dimensions.
         */
        void set_page_format(model::DocraftPageOrientation orientation);

    private:
        std::unique_ptr<DocraftCursor> cursor_;
        float page_width_ = 0.0F;
        float page_height_ = 0.0F;
        float current_rect_width_ = 0.0F;
    };
} // namespace docraft::services


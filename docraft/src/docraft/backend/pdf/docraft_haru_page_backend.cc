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

#include "docraft/backend/pdf/docraft_haru_page_backend.h"

#include <format>
#include <stdexcept>
#include <string>

#include <hpdf.h>

namespace docraft::backend::pdf {
    namespace {
        HPDF_PageSizes to_hpdf_size(model::DocraftPageSize size) {
            switch (size) {
                case model::DocraftPageSize::kA3:
                    return HPDF_PAGE_SIZE_A3;
                case model::DocraftPageSize::kA5:
                    return HPDF_PAGE_SIZE_A5;
                case model::DocraftPageSize::kLetter:
                    return HPDF_PAGE_SIZE_LETTER;
                case model::DocraftPageSize::kLegal:
                    return HPDF_PAGE_SIZE_LEGAL;
                case model::DocraftPageSize::kA4:
                default:
                    return HPDF_PAGE_SIZE_A4;
            }
        }

        HPDF_PageDirection to_hpdf_direction(model::DocraftPageOrientation orientation) {
            switch (orientation) {
                case model::DocraftPageOrientation::kLandscape:
                    return HPDF_PAGE_LANDSCAPE;
                case model::DocraftPageOrientation::kPortrait:
                default:
                    return HPDF_PAGE_PORTRAIT;
            }
        }
    } // namespace

    DocraftHaruPageBackend::DocraftHaruPageBackend(
        const std::shared_ptr<DocraftHaruSharedState> &state) : state_(state) {
        if (state_) {
            // Register self as the page operations provider for capability backends.
            // LIFETIME CONTRACT: This provider remains valid for the entire lifetime of state_.
            state_->set_page_operations_provider(this);
        }
    }

    DocraftHaruPageBackend::~DocraftHaruPageBackend() {
        if (state_ && state_->page_operations_provider == this) {
            state_->clear_page_operations_provider();
        }
    }

    float DocraftHaruPageBackend::page_width() const {
        return HPDF_Page_GetWidth(current_page());
    }

    float DocraftHaruPageBackend::page_height() const {
        return HPDF_Page_GetHeight(current_page());
    }

    void DocraftHaruPageBackend::add_new_page() {
        HPDF_Page new_page = HPDF_AddPage(state_ ? state_->pdf : nullptr);
        if (!new_page) {
            throw std::runtime_error("Failed to create a new page");
        }
        apply_page_format(new_page);
        state_->add_page(new_page);
        state_->edit_current_page_index() = state_->page_count() - 1;
    }

    void DocraftHaruPageBackend::move_to_next_page() {
        if (state_->current_page_index() + 1 < state_->page_count()) {
            state_->edit_current_page_index() = state_->current_page_index() + 1;
            return;
        }
        throw std::runtime_error("Already at the last page, cannot move to next page");
    }

    void DocraftHaruPageBackend::go_to_page(std::size_t page_number) {
        if (page_number < state_->page_count()) {
            state_->edit_current_page_index() = page_number;
            return;
        }
        throw std::runtime_error(std::format("Invalid page number: {}. Total pages: {}", page_number + 1,
                                             state_->page_count()));
    }

    void DocraftHaruPageBackend::go_to_first_page() {
        if (state_->page_count() == 0) {
            throw std::runtime_error("No pages in document");
        }
        state_->edit_current_page_index() = 0;
    }

    void DocraftHaruPageBackend::go_to_previous_page() {
        if (state_->current_page_index() == 0) {
            throw std::runtime_error("Already at the first page, cannot move to previous page");
        }
        state_->edit_current_page_index() = state_->current_page_index() - 1;
    }

    void DocraftHaruPageBackend::go_to_last_page() {
        if (state_->page_count() == 0) {
            throw std::runtime_error("No pages in document");
        }
        state_->edit_current_page_index() = state_->page_count() - 1;
    }

    void DocraftHaruPageBackend::set_page_format(model::DocraftPageSize size,
                                                 model::DocraftPageOrientation orientation) {
        state_->edit_page_size() = to_hpdf_size(size);
        state_->edit_page_direction() = to_hpdf_direction(orientation);
        for (auto &page: state_->edit_pages()) {
            if (page) {
                apply_page_format(page);
            }
        }
    }

    std::size_t DocraftHaruPageBackend::current_page_number() const {
        return state_->current_page_index() + 1;
    }

    std::size_t DocraftHaruPageBackend::total_page_count() const {
        return state_->page_count();
    }

    HPDF_Page DocraftHaruPageBackend::current_page() const {
        if (state_->page_count() == 0) {
            throw std::runtime_error("No pages in document");
        }
        if (state_->current_page_index() >= state_->page_count()) {
            throw std::runtime_error("Current page index is out of bounds");
        }
        return state_->pages()[state_->current_page_index()];
    }

    std::size_t DocraftHaruPageBackend::current_page_index() const {
        if (state_->page_count() == 0) {
            throw std::runtime_error("No pages in document");
        }
        return state_->current_page_index();
    }

    void DocraftHaruPageBackend::apply_page_format(HPDF_Page page) const {
        HPDF_Page_SetSize(page, state_->page_size(), state_->page_direction());
    }
} // namespace docraft::backend::pdf


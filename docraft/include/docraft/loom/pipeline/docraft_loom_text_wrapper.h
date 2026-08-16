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

#include <memory>
#include <string>
#include <vector>

#include "docraft/backend/docraft_backend_providers_factory.h"

namespace docraft::loom::pipeline {
    /**
     * @class DocraftLoomTextWrapper
     *
     * @brief Greedily word-wraps text into lines that each fit within a given width,
     * measuring against a text rendering backend. Extracted out of
     * DocraftLoomMeasureProcessor so the wrapping algorithm itself (paragraph
     * splitting, greedy word-fitting, character-level fallback for an
     * over-wide word) is testable and reasoned about independently of the
     * visitor that drives it.
     */
    class DocraftLoomTextWrapper
    {
    public:
        explicit DocraftLoomTextWrapper(const std::shared_ptr<backend::IDocraftTextRenderingBackend>& text_backend);

        /**
         * @brief Greedily word-wraps text into lines that each fit within max_width,
         * falling back to a character-level split for a single word wider than
         * max_width on its own. Splits on explicit '\n' first, then wraps each resulting
         * paragraph by words.
         */
        std::vector<std::string> wrap(const std::string_view &text, float max_width, const std::string &font_name,
                                      float font_size) const;

    private:
        /**
         * @brief Word-wraps a single paragraph (no embedded newlines) into `lines`.
         */
        void wrap_paragraph(const std::string& paragraph, float max_width, const std::string& font_name,
                            float font_size, std::vector<std::string>& lines) const;

        /**
         * @brief Splits a single word wider than max_width into as many character-level
         * chunks as needed, appending each to `lines`. Never splits a multi-byte UTF-8
         * codepoint across two chunks.
         */
        void split_word_by_character(const std::string& word, float max_width, const std::string& font_name,
                                     float font_size, std::vector<std::string>& lines) const;

        std::shared_ptr<docraft::backend::IDocraftTextRenderingBackend> text_backend_;
    };
} // namespace docraft::loom::pipeline

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

#include "docraft/loom/pipeline/docraft_loom_text_wrapper.h"

#include <algorithm>
#include <sstream>

#include "docraft/utils/docraft_utf8.h"

namespace docraft::loom::pipeline {
    DocraftLoomTextWrapper::DocraftLoomTextWrapper(
        const std::shared_ptr<backend::IDocraftTextRenderingBackend> &text_backend)
        : text_backend_(text_backend) {
    }

    void DocraftLoomTextWrapper::split_word_by_character(const std::string &word, float max_width,
                                                         const std::string &font_name, float font_size,
                                                         std::vector<std::string> &lines) const {
        if (word.empty()) {
            return;
        }
        std::size_t start = 0;
        while (start < word.length()) {
            std::size_t probe_end = std::min(
                start + docraft::utils::utf8_codepoint_byte_length(static_cast<unsigned char>(word[start])),
                word.length());
            std::size_t last_fit_end = start;
            while (true) {
                if (const std::string candidate = word.substr(start, probe_end - start);
                    text_backend_->measure_text_width(candidate, font_name, font_size) > max_width) {
                    break;
                }
                last_fit_end = probe_end;
                if (probe_end >= word.length()) {
                    break;
                }
                probe_end = std::min(
                    probe_end + docraft::utils::utf8_codepoint_byte_length(static_cast<unsigned char>(word[probe_end])),
                    word.length());
            }
            if (last_fit_end == start) {
                // Not even one glyph fits -- take one full codepoint anyway to
                // guarantee progress, without splitting a multi-byte UTF-8 sequence.
                last_fit_end = std::min(
                    start + docraft::utils::utf8_codepoint_byte_length(static_cast<unsigned char>(word[start])),
                    word.length());
            }
            lines.push_back(word.substr(start, last_fit_end - start));
            start = last_fit_end;
        }
    }

    void DocraftLoomTextWrapper::wrap_paragraph(const std::string &paragraph, float max_width,
                                                const std::string &font_name, float font_size,
                                                std::vector<std::string> &lines) const {
        std::istringstream iss(paragraph);
        std::string word;
        std::string current_line;
        while (iss >> word) {
            if (current_line.empty()) {
                if (text_backend_->measure_text_width(word, font_name, font_size) <= max_width) {
                    current_line = word;
                } else {
                    split_word_by_character(word, max_width, font_name, font_size, lines);
                }
                continue;
            }
            std::string candidate = current_line;
            candidate=candidate.append(" ").append(word);
            if (text_backend_->measure_text_width(candidate, font_name, font_size) <= max_width) {
                current_line = candidate;
            } else {
                lines.push_back(current_line);
                current_line.clear();
                if (text_backend_->measure_text_width(word, font_name, font_size) <= max_width) {
                    current_line = word;
                } else {
                    split_word_by_character(word, max_width, font_name, font_size, lines);
                }
            }
        }
        if (!current_line.empty()) {
            lines.push_back(current_line);
        }
    }

    std::vector<std::string> DocraftLoomTextWrapper::wrap(const std::string_view &text, float max_width,
                                                          const std::string &font_name, float font_size) const {
        std::vector<std::string> lines;

        // Split on explicit newlines first, then word-wrap each paragraph.
        for (std::size_t start = 0; start < text.length();) {
            std::size_t end = text.find('\n', start);
            if (end == std::string::npos) {
                end = text.length();
            }
            wrap_paragraph(std::string(text.substr(start, end - start)), max_width, font_name, font_size, lines);
            start = end + 1;
        }
        if (lines.empty()) {
            lines.emplace_back();
        }
        return lines;
    }
} // namespace docraft::loom::pipeline

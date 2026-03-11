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

#include <cstddef>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace docraft {
    class DocraftDocument;
}

namespace docraft::utils {
    /**
     * @brief Extracts document keywords using term-frequency statistics.
     *
     * Words are tokenized from text nodes, normalized to lowercase, filtered
     * by min length and stopwords, then ranked by descending frequency.
     */
    class DOCRAFT_LIB DocraftKeywordExtractor {
    public:
        struct Config {
            std::size_t max_keywords = 10;
            std::size_t min_length = 4;
            std::vector<std::string> stop_word_languages = {"it", "en", "fr", "de", "es"};
            std::unordered_set<std::string> stop_words;
        };

        DocraftKeywordExtractor();
        explicit DocraftKeywordExtractor(Config config);

        /**
         * @brief Extract top keywords sorted by statistical frequency.
         * @param document Source document.
         * @return Top keyword list.
         */
        [[nodiscard]] std::vector<std::string> extract(const DocraftDocument &document) const;

        /**
         * @brief Extract keywords with their occurrence counts.
         * @param document Source document.
         * @return Sorted pairs <keyword, frequency>.
         */
        [[nodiscard]] std::vector<std::pair<std::string, std::size_t>> extract_with_frequency(
            const DocraftDocument &document) const;

    private:
        static std::unordered_set<std::string> language_stop_words(const std::vector<std::string> &languages);
        static std::string normalize_word(const std::string &word);
        [[nodiscard]] bool is_stop_word(const std::string &word) const;
        [[nodiscard]] bool is_candidate(const std::string &word) const;

        Config config_;
    };
} // namespace docraft::utils

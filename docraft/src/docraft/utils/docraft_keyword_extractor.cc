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

#include "docraft/utils/docraft_keyword_extractor.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "docraft/docraft_document.h"
#include "docraft/model/docraft_text.h"

namespace docraft::utils {
    namespace {
        std::string to_lower_ascii(const std::string &value) {
            std::string lowered = value;
            for (char &ch: lowered) {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
            return lowered;
        }
    } // namespace

    DocraftKeywordExtractor::DocraftKeywordExtractor() : DocraftKeywordExtractor(Config{}) {
    }

    DocraftKeywordExtractor::DocraftKeywordExtractor(Config config) : config_(std::move(config)) {
        std::unordered_set<std::string> merged_stop_words = language_stop_words(config_.stop_word_languages);
        for (const auto &word: config_.stop_words) {
            merged_stop_words.insert(to_lower_ascii(word));
        }
        config_.stop_words = std::move(merged_stop_words);
        if (config_.max_keywords == 0U) {
            config_.max_keywords = 1U;
        }
        if (config_.min_length == 0U) {
            config_.min_length = 1U;
        }
    }

    std::vector<std::string> DocraftKeywordExtractor::extract(const DocraftDocument &document) const {
        const auto ranked = extract_with_frequency(document);
        std::vector<std::string> result;
        result.reserve(ranked.size());
        for (const auto &[word, _]: ranked) {
            result.push_back(word);
        }
        return result;
    }

    std::vector<std::pair<std::string, std::size_t>> DocraftKeywordExtractor::extract_with_frequency(
        const DocraftDocument &document) const {
        std::unordered_map<std::string, std::size_t> frequency_map;

        auto register_word = [&](const std::string &word) {
            if (is_candidate(word)) {
                ++frequency_map[word];
            }
        };

        document.traverse_dom([&](const std::shared_ptr<model::DocraftNode> &node, DocraftDomTraverseOp operation) {
            if (operation != DocraftDomTraverseOp::kEnter) {
                return;
            }
            const auto text_node = std::dynamic_pointer_cast<model::DocraftText>(node);
            if (!text_node) {
                return;
            }

            std::string token;
            for (const char ch: text_node->text()) {
                const unsigned char raw = static_cast<unsigned char>(ch);
                if (std::isalnum(raw)) {
                    token.push_back(static_cast<char>(std::tolower(raw)));
                } else if (!token.empty()) {
                    register_word(token);
                    token.clear();
                }
            }
            if (!token.empty()) {
                register_word(token);
            }
        });

        std::vector<std::pair<std::string, std::size_t>> ranked(
            frequency_map.begin(), frequency_map.end());
        std::sort(ranked.begin(), ranked.end(), [](const auto &left, const auto &right) {
            if (left.second != right.second) {
                return left.second > right.second;
            }
            return left.first < right.first;
        });
        if (ranked.size() > config_.max_keywords) {
            ranked.resize(config_.max_keywords);
        }
        return ranked;
    }

    std::unordered_set<std::string> DocraftKeywordExtractor::language_stop_words(
        const std::vector<std::string> &languages) {
        static const std::unordered_map<std::string, std::unordered_set<std::string>> stop_words_by_language = {
            {"en", {
                "the", "a", "an", "and", "or", "of", "to", "in", "on", "for", "with", "by", "is", "are", "be",
                "that", "this", "it", "as", "at", "from", "was", "were", "will", "can", "if", "then", "than",
                "into", "about", "not", "yes", "you", "your", "our", "we", "i", "he", "she", "they", "them",
                "their", "its", "do", "does", "did", "have", "has", "had"
            }},
            {"it", {
                "il", "lo", "la", "i", "gli", "le", "un", "uno", "una", "e", "o", "di", "a", "da", "in", "con",
                "su", "per", "tra", "fra", "che", "del", "della", "dei", "degli", "delle", "al", "allo", "alla",
                "ai", "agli", "alle", "dal", "dallo", "dalla", "dai", "dagli", "dalle", "nel", "nello", "nella",
                "nei", "negli", "nelle", "sul", "sullo", "sulla", "sui", "sugli", "sulle", "come", "piu", "meno",
                "non", "si", "ma", "anche", "ha", "hanno", "era", "sono", "essere", "avere", "io", "tu", "lui",
                "lei", "noi", "voi", "loro"
            }},
            {"fr", {
                "le", "la", "les", "un", "une", "des", "du", "de", "d", "et", "ou", "a", "au", "aux", "dans",
                "sur", "sous", "pour", "par", "avec", "sans", "que", "qui", "quoi", "dont", "ou", "est", "sont",
                "etre", "avoir", "je", "tu", "il", "elle", "nous", "vous", "ils", "elles", "ce", "cet", "cette",
                "ces", "se", "sa", "son", "ses", "ne", "pas", "plus", "moins", "comme"
            }},
            {"de", {
                "der", "die", "das", "ein", "eine", "einer", "einem", "einen", "und", "oder", "zu", "in", "auf",
                "mit", "von", "für", "fur", "bei", "aus", "ist", "sind", "sein", "haben", "ich", "du", "er", "sie",
                "wir", "ihr", "nicht", "ja", "nein", "den", "dem", "des", "als", "wie", "auch", "noch", "nur",
                "dass", "wenn", "dann", "aber", "an", "am", "im"
            }},
            {"es", {
                "el", "la", "los", "las", "un", "una", "unos", "unas", "y", "o", "de", "del", "al", "a", "en",
                "con", "sin", "por", "para", "que", "como", "es", "son", "ser", "estar", "tener", "yo", "tu", "el",
                "ella", "nosotros", "vosotros", "ellos", "ellas", "su", "sus", "se", "no", "si", "mas", "menos",
                "tambien", "pero", "cuando", "donde", "desde", "hasta"
            }}
        };

        std::unordered_set<std::string> merged_stop_words;
        for (const auto &language: languages) {
            const std::string normalized_language = to_lower_ascii(language);
            const auto it = stop_words_by_language.find(normalized_language);
            if (it == stop_words_by_language.end()) {
                continue;
            }
            merged_stop_words.insert(it->second.begin(), it->second.end());
        }
        return merged_stop_words;
    }

    std::string DocraftKeywordExtractor::normalize_word(const std::string &word) {
        return to_lower_ascii(word);
    }

    bool DocraftKeywordExtractor::is_stop_word(const std::string &word) const {
        return config_.stop_words.find(normalize_word(word)) != config_.stop_words.end();
    }

    bool DocraftKeywordExtractor::is_candidate(const std::string &word) const {
        if (word.size() < config_.min_length) {
            return false;
        }
        bool has_alpha = false;
        for (const char ch: word) {
            if (std::isalpha(static_cast<unsigned char>(ch))) {
                has_alpha = true;
                break;
            }
        }
        if (!has_alpha) {
            return false;
        }
        return !is_stop_word(word);
    }
} // namespace docraft::utils

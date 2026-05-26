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
#include <string>
#include <memory>
#include <vector>

#include "docraft/docraft_document_metadata.h"
#include "docraft/utils/docraft_keyword_extractor.h"

namespace docraft::model {
    class DocraftSettings;
    class DocraftNode;
}

namespace docraft::templating {
    class DocraftTemplateEngine;
}

namespace docraft::management {
    /**
     * @brief Manages document metadata, settings, and configuration.
     *
     * Handles title, path, metadata, keywords, settings, and template engine.
     */
    class DOCRAFT_LIB DocraftDocumentConfig {
    public:
        /**
         * @brief Sets the document title.
         * @param document_title New title value.
         */
        void set_document_title(const std::string &document_title);

        /**
         * @brief Returns the current document title.
         * @return Document title string.
         */
        [[nodiscard]] const std::string &document_title() const;

        [[nodiscard]] std::string &edit_document_title();

        /**
         * @brief Sets the output directory where the rendered file will be saved.
         * @param document_path Output directory path.
         */
        void set_document_path(const std::string &document_path);

        /**
         * @brief Returns the current output directory path.
         * @return Output directory path.
         */
        [[nodiscard]] const std::string &document_path() const;

        [[nodiscard]] std::string &edit_document_path();

        /**
         * @brief Sets document settings (fonts, etc.).
         * @param settings Settings node to apply.
         */
        void set_settings(const std::shared_ptr<model::DocraftSettings> &settings);

        /**
         * @brief Returns the current settings object.
         * @return Shared pointer to settings or nullptr if not set.
         */
        [[nodiscard]] std::shared_ptr<const model::DocraftSettings> settings() const;

        [[nodiscard]] std::shared_ptr<model::DocraftSettings> edit_settings();

        /**
         * @brief Sets document metadata values.
         * @param metadata Metadata values supported by library.
         */
        void set_document_metadata(const DocraftDocumentMetadata &metadata);

        /**
         * @brief Returns current document metadata values.
         * @return Metadata object.
         */
        [[nodiscard]] const DocraftDocumentMetadata &document_metadata() const;

        /**
         * @brief Enables or disables automatic keyword extraction for metadata.
         * @param enabled true to enable, false to disable.
         */
        void enable_auto_keywords(bool enabled = true);

        /**
         * @brief Returns whether automatic keyword extraction is enabled.
         */
        [[nodiscard]] bool auto_keywords_enabled() const;

        /**
         * @brief Sets configuration for automatic keyword extraction.
         * @param config Extractor configuration.
         */
        void set_auto_keywords_config(const utils::DocraftKeywordExtractor::Config &config);

        /**
         * @brief Returns the current automatic keyword extraction configuration.
         */
        [[nodiscard]] const utils::DocraftKeywordExtractor::Config &auto_keywords_config() const;

        /**
         * @brief Extracts keywords from a set of nodes and merges them into metadata.
         * @param nodes The nodes to extract keywords from (typically document DOM).
         *
         * No-op when auto-keyword extraction is disabled.
         */
        void refresh_auto_keywords(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes);

        void set_document_template_engine(const std::shared_ptr<templating::DocraftTemplateEngine> &template_engine);

        [[nodiscard]] std::shared_ptr<const templating::DocraftTemplateEngine> document_template_engine() const;

        [[nodiscard]] std::shared_ptr<templating::DocraftTemplateEngine> edit_document_template_engine();

    private:
        std::string document_title_ = "Untitled Document";
        std::string document_path_;
        std::shared_ptr<model::DocraftSettings> settings_;
        DocraftDocumentMetadata metadata_;
        bool auto_keywords_enabled_ = false;
        utils::DocraftKeywordExtractor::Config auto_keywords_config_{};
        std::shared_ptr<templating::DocraftTemplateEngine> template_engine_;
    };
}


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

#include "docraft/management/docraft_document_config.h"
#include "docraft/model/docraft_settings.h"
#include "docraft/templating/docraft_template_engine.h"

namespace docraft::management {
    void DocraftDocumentConfig::set_document_title(const std::string &document_title) {
        document_title_ = document_title;
        metadata_.set_title(document_title);
    }

    const std::string &DocraftDocumentConfig::document_title() const {
        return document_title_;
    }

    std::string &DocraftDocumentConfig::edit_document_title() {
        return document_title_;
    }

    void DocraftDocumentConfig::set_document_path(const std::string &document_path) {
        document_path_ = document_path;
    }

    const std::string &DocraftDocumentConfig::document_path() const {
        return document_path_;
    }

    std::string &DocraftDocumentConfig::edit_document_path() {
        return document_path_;
    }

    void DocraftDocumentConfig::set_settings(const std::shared_ptr<model::DocraftSettings> &settings) {
        settings_ = settings;
    }

    std::shared_ptr<const model::DocraftSettings> DocraftDocumentConfig::settings() const {
        return settings_;
    }

    std::shared_ptr<model::DocraftSettings> DocraftDocumentConfig::edit_settings() {
        return settings_;
    }

    void DocraftDocumentConfig::set_document_metadata(const DocraftDocumentMetadata &metadata) {
        metadata_ = metadata;
        if (metadata_.title().has_value()) {
            document_title_ = metadata_.title().value();
        } else {
            metadata_.set_title(document_title_);
        }
    }

    const DocraftDocumentMetadata &DocraftDocumentConfig::document_metadata() const {
        return metadata_;
    }

    void DocraftDocumentConfig::enable_auto_keywords(bool enabled) {
        auto_keywords_enabled_ = enabled;
    }

    bool DocraftDocumentConfig::auto_keywords_enabled() const {
        return auto_keywords_enabled_;
    }

    void DocraftDocumentConfig::set_auto_keywords_config(const utils::DocraftKeywordExtractor::Config &config) {
        auto_keywords_config_ = config;
    }

    const utils::DocraftKeywordExtractor::Config &DocraftDocumentConfig::auto_keywords_config() const {
        return auto_keywords_config_;
    }

    void DocraftDocumentConfig::set_document_template_engine(
        const std::shared_ptr<templating::DocraftTemplateEngine> &template_engine) {
        template_engine_ = template_engine;
    }

    std::shared_ptr<const templating::DocraftTemplateEngine> DocraftDocumentConfig::document_template_engine() const {
        return template_engine_;
    }

    std::shared_ptr<templating::DocraftTemplateEngine> DocraftDocumentConfig::edit_document_template_engine() {
        return template_engine_;
    }
} // docraft::management


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

#include "docraft/docraft_document_metadata.h"

namespace docraft {
    void DocraftDocumentMetadata::set_author(const std::string &author) {
        author_ = author;
    }

    void DocraftDocumentMetadata::set_creator(const std::string &creator) {
        creator_ = creator;
    }

    void DocraftDocumentMetadata::set_producer(const std::string &producer) {
        producer_ = producer;
    }

    void DocraftDocumentMetadata::set_title(const std::string &title) {
        title_ = title;
    }

    void DocraftDocumentMetadata::set_subject(const std::string &subject) {
        subject_ = subject;
    }

    void DocraftDocumentMetadata::set_keywords(const std::string &keywords) {
        keywords_ = keywords;
    }

    void DocraftDocumentMetadata::set_trapped(const std::string &trapped) {
        trapped_ = trapped;
    }

    void DocraftDocumentMetadata::set_gts_pdfx(const std::string &gts_pdfx) {
        gts_pdfx_ = gts_pdfx;
    }

    void DocraftDocumentMetadata::set_creation_date(const DateTime &creation_date) {
        creation_date_ = creation_date;
    }

    void DocraftDocumentMetadata::set_modification_date(const DateTime &modification_date) {
        modification_date_ = modification_date;
    }

    void DocraftDocumentMetadata::clear_author() {
        author_.reset();
    }

    void DocraftDocumentMetadata::clear_creator() {
        creator_.reset();
    }

    void DocraftDocumentMetadata::clear_producer() {
        producer_.reset();
    }

    void DocraftDocumentMetadata::clear_title() {
        title_.reset();
    }

    void DocraftDocumentMetadata::clear_subject() {
        subject_.reset();
    }

    void DocraftDocumentMetadata::clear_keywords() {
        keywords_.reset();
    }

    void DocraftDocumentMetadata::clear_trapped() {
        trapped_.reset();
    }

    void DocraftDocumentMetadata::clear_gts_pdfx() {
        gts_pdfx_.reset();
    }

    void DocraftDocumentMetadata::clear_creation_date() {
        creation_date_.reset();
    }

    void DocraftDocumentMetadata::clear_modification_date() {
        modification_date_.reset();
    }

    const std::optional<std::string> &DocraftDocumentMetadata::author() const {
        return author_;
    }

    const std::optional<std::string> &DocraftDocumentMetadata::creator() const {
        return creator_;
    }

    const std::optional<std::string> &DocraftDocumentMetadata::producer() const {
        return producer_;
    }

    const std::optional<std::string> &DocraftDocumentMetadata::title() const {
        return title_;
    }

    const std::optional<std::string> &DocraftDocumentMetadata::subject() const {
        return subject_;
    }

    const std::optional<std::string> &DocraftDocumentMetadata::keywords() const {
        return keywords_;
    }

    const std::optional<std::string> &DocraftDocumentMetadata::trapped() const {
        return trapped_;
    }

    const std::optional<std::string> &DocraftDocumentMetadata::gts_pdfx() const {
        return gts_pdfx_;
    }

    const std::optional<DocraftDocumentMetadata::DateTime> &DocraftDocumentMetadata::creation_date() const {
        return creation_date_;
    }

    const std::optional<DocraftDocumentMetadata::DateTime> &DocraftDocumentMetadata::modification_date() const {
        return modification_date_;
    }
} // namespace docraft

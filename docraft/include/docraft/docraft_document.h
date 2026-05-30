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
#include <vector>
#include <memory>
#include <functional>
#include <string>

#include "docraft/docraft_document_context.h"
#include "docraft/docraft_document_metadata.h"
#include "docraft/management/docraft_document_config.h"
#include "docraft/management/docraft_document_query.h"
#include "docraft/model/docraft_node.h"
#include "docraft/model/docraft_settings.h"
#include "docraft/templating/docraft_template_engine.h"
#include "docraft/utils/docraft_keyword_extractor.h"

namespace docraft {
    // Keep enum in docraft namespace for backward compatibility
    enum class DocraftDomTraverseOp {
        kEnter,
        kExit
    };

    /**
     * @brief High-level document container that owns document configuration and the DOM node list.
     *
     * DocraftDocument is the primary API surface for building a document tree,
     * and invoking rendering. Configuration (metadata, settings, keywords) is delegated
     * to DocraftDocumentConfig for single responsibility.
     */
    class DOCRAFT_LIB DocraftDocument {
    public:
        /**
         * @brief Creates a document with an optional title.
         * @param document_title Human-readable title for the document metadata.
         */
        explicit DocraftDocument(const std::string &document_title = "Untitled Document");

        /**
         * @brief Virtual destructor.
         */
        virtual ~DocraftDocument() = default;

        /**
         * @brief Adds a node to the document DOM.
         * @param node Node to append to the document.
         */
        void add_node(const std::shared_ptr<model::DocraftNode> &node);

        /**
         * @brief Applies document settings to the underlying rendering context.
         */
        void configure_document_settings();

        /**
         * @brief Applies template processing to the document DOM using the configured template engine.
         */
        void template_document();

        /**
         * @brief Renders the document using the configured context and renderer.
         */
        void render();

        /**
         * @brief Overrides the backend used during render.
         *
         * Passing nullptr resets to the default backend.
         * @param backend Backend implementation.
         */
        void set_backend(const std::shared_ptr<backend::IDocraftBackend> &backend);

        /**
         * @brief Returns the document DOM nodes.
         * @return Vector of root nodes.
         */
        [[nodiscard]] std::vector<std::shared_ptr<const model::DocraftNode> > nodes() const;

        [[nodiscard]] std::vector<std::shared_ptr<model::DocraftNode> > &edit_nodes();

        /**
         * @brief Traverses the document DOM and executes a callback on each node.
         * @param callback Function called for each node and operation (enter/exit).
         */
        void traverse_dom(
            const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback)
        const;

        /**
         * @brief Returns the document configuration manager.
         * @return Reference to the configuration container.
         */
        management::DocraftDocumentConfig &edit_config();

        [[nodiscard]] const management::DocraftDocumentConfig &config() const;

        /**
         * @brief Returns the document context used for rendering.
         * @return Shared pointer to the rendering context.
         */
        [[nodiscard]] std::shared_ptr<DocraftDocumentContext> edit_context();

        [[nodiscard]] std::shared_ptr<const DocraftDocumentContext> context() const;

        // Backward compatibility delegates to config_
        void set_document_title(const std::string &document_title);

        [[nodiscard]] const std::string &document_title() const;

        void set_document_path(const std::string &document_path);

        [[nodiscard]] const std::string &document_path() const;

        void set_settings(const std::shared_ptr<model::DocraftSettings> &settings);

        [[nodiscard]] std::shared_ptr<const model::DocraftSettings> settings() const;

        void set_document_metadata(const DocraftDocumentMetadata &metadata);

        [[nodiscard]] const DocraftDocumentMetadata &document_metadata() const;

        // Backward compatibility: DOM query delegates
        [[nodiscard]] std::vector<std::shared_ptr<const model::DocraftNode> > find_by_name(
            const std::string &name) const;

        [[nodiscard]] std::vector<std::shared_ptr<model::DocraftNode> > take_by_name(const std::string &name);

        [[nodiscard]] std::shared_ptr<const model::DocraftNode> find_first_by_name(const std::string &name) const;

        [[nodiscard]] std::shared_ptr<model::DocraftNode> take_first_by_name(const std::string &name);

        [[nodiscard]] std::shared_ptr<const model::DocraftNode> find_last_by_name(const std::string &name) const;

        [[nodiscard]] std::shared_ptr<model::DocraftNode> take_last_by_name(const std::string &name);

        template<typename T>
        [[nodiscard]] std::vector<std::shared_ptr<const T> > find_by_type() const;

        template<typename T>
        [[nodiscard]] std::vector<std::shared_ptr<T> > take_by_type();

        // Backward compatibility: config shortcuts
        void enable_auto_keywords(bool enabled = true);

        [[nodiscard]] bool auto_keywords_enabled() const;

        void set_auto_keywords_config(const utils::DocraftKeywordExtractor::Config &config);

        [[nodiscard]] const utils::DocraftKeywordExtractor::Config &auto_keywords_config() const;

        void set_document_template_engine(const std::shared_ptr<templating::DocraftTemplateEngine> &template_engine);

        [[nodiscard]] std::shared_ptr<const templating::DocraftTemplateEngine> document_template_engine() const;

        [[nodiscard]] std::shared_ptr<templating::DocraftTemplateEngine> edit_document_template_engine();

        void refresh_auto_keywords();

    private:
        void traverse_node(
            const std::shared_ptr<model::DocraftNode> &node,
            const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback)
        const;

        /**
         * @brief Handles rendering of a single node, applying settings and invoking the renderer.
         * @param node
         */
        void handle_node_rendering(const std::shared_ptr<model::DocraftNode> &node);

        std::shared_ptr<DocraftDocumentContext> context_;
        std::vector<std::shared_ptr<model::DocraftNode> > dom_;
        management::DocraftDocumentConfig config_;
    };
}

#include "docraft_document.hpp"

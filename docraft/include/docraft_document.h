#pragma once
#include <vector>
#include <memory>

#include "docraft_document_context.h"
#include "model/docraft_node.h"
#include "model/docraft_settings.h"

namespace docraft {
    /**
     * @brief High-level document container that owns settings, title, and the DOM node list.
     *
     * DocraftDocument is the primary API surface for building a document tree,
     * configuring settings, and invoking rendering.
     */
    class DocraftDocument {
        public:
        /**
         * @brief Creates a document with an optional title.
         * @param document_title Human-readable title for the document metadata.
         */
        DocraftDocument(std::string document_title="Untitled Document");
        /**
         * @brief Virtual destructor.
         */
        virtual ~DocraftDocument()=default;
        /**
         * @brief Adds a node to the document DOM.
         * @param node Node to append to the document.
         */
        void add_node(const std::shared_ptr<model::DocraftNode>& node);

        /**
         * @brief Applies document settings to the underlying rendering context.
         */
        void configure_document_settings() const;

        /**
         * @brief Renders the document using the configured context and renderer.
         */
        void render();
        /**
         * @brief Sets the document title.
         * @param document_title New title value.
         */
        void set_document_title(const std::string &document_title);
        /**
         * @brief Returns the current document title.
         * @return Document title string.
         */
        std::string document_title();
        /**
         * @brief Sets document settings (fonts, etc.).
         * @param settings Settings node to apply.
         */
        void set_settings(const std::shared_ptr<model::DocraftSettings> &settings);
        /**
         * @brief Returns the current settings object.
         * @return Shared pointer to settings or nullptr if not set.
         */
        std::shared_ptr<model::DocraftSettings> settings() const;
    private:
        std::shared_ptr<DocraftDocumentContext> pdf_context_;
        std::shared_ptr<model::DocraftSettings> settings_;
        std::string document_title_;
        std::vector<std::shared_ptr<model::DocraftNode>> dom_;
    };
}

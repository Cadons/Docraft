#pragma once
#include <vector>
#include <memory>
#include <functional>

#include "docraft_document_context.h"
#include "model/docraft_node.h"
#include "model/docraft_settings.h"
#include "templating/docraft_template_engine.h"

namespace docraft {
    enum class DocraftDomTraverseOp {
        kEnter,
        kExit
    };
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
        DocraftDocument(std::string document_title = "Untitled Document");

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

        void set_document_template_engine(const std::shared_ptr<templating::DocraftTemplateEngine> &template_engine);

        std::shared_ptr<templating::DocraftTemplateEngine> document_template_engine() const;
        /**
         * @brief Returns the document DOM nodes.
         * @return Vector of root nodes.
         */
        const std::vector<std::shared_ptr<model::DocraftNode>> &nodes() const;
        /**
         * @brief Finds nodes by name in the document DOM.
         * @param name Node name to search for.
         * @return Vector of nodes matching the name, or empty vector if none found.
         */
        std::vector<std::shared_ptr<model::DocraftNode>> get_by_name(const std::string &name) const;
        /**
         * @brief Finds the first node by name in the document DOM.
         * @param name Node name to search for.
         * @return Shared pointer to the first matching node, or nullptr if not found.
         */
        std::shared_ptr<model::DocraftNode> get_first_by_name(const std::string &name) const;
        /**
         * @brief Finds the last node by name in the document DOM.
         * @param name Node name to search for.
         * @return Shared pointer to the last matching node, or nullptr if not found.
         */
        std::shared_ptr<model::DocraftNode> get_last_by_name(const std::string &name) const;
        /**
         * @brief Finds nodes by type in the document DOM.
         * @tparam T Node type to search for.
         * @return Vector of nodes matching the type, or empty vector if none found.
         */
        template <typename T>
        std::vector<std::shared_ptr<T>> get_by_type() const;
        /**
         * @brief Traverses the document DOM and executes a callback on each node.
         * @param callback Function called for each node and operation (enter/exit).
         */
        void traverse_dom(
            const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback) const;

    private:
        void traverse_node(
            const std::shared_ptr<model::DocraftNode> &node,
            const std::function<void(const std::shared_ptr<model::DocraftNode> &, DocraftDomTraverseOp)> &callback) const;
        std::shared_ptr<DocraftDocumentContext> pdf_context_;
        std::shared_ptr<model::DocraftSettings> settings_;
        std::string document_title_;
        std::vector<std::shared_ptr<model::DocraftNode> > dom_;
        std::shared_ptr<templating::DocraftTemplateEngine> template_engine_;
    };
}

#include "docraft_document.hpp"

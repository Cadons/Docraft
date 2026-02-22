#pragma once

#include "docraft_lib.h"
#include "docraft_children_container_node.h"
#include "docraft_node.h"
#include "model/i_docraft_clonable.h"

namespace docraft::model {
    /**
     * @brief Represents a foreach loop in the document, allowing for dynamic generation of content based on a collection of items.
     * The foreach node should have a "model" attribute that specifies the collection to iterate over,
     * and its children will be rendered for each item in the collection with template variables available for the current item.
     */
    class DOCRAFT_LIB DocraftForeach : public DocraftChildrenContainerNode, public IDocraftClonable {
    public:
        DocraftForeach() = default;
        DocraftForeach(const DocraftForeach &) = default;

        ~DocraftForeach() override = default;

        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

        /**
         * @brief Clones the foreach node, its rendered children, and its template nodes.
         * @return Shared pointer to cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;

        /**
         * @brief Sets the template nodes that will be rendered for each item in the collection.
         * @param nodes The template nodes to render for each item.
         */
        void set_template_nodes(const std::vector<std::shared_ptr<DocraftNode> > &nodes);

        /**
         * @brief Gets the template nodes that will be rendered for each item in the collection.
         * @return The template nodes to render for each item.
         */
        const std::vector<std::shared_ptr<DocraftNode> > &template_nodes() const;

        /**
         * @brief Sets the model attribute that specifies the collection to iterate over.
         * @param model The model attribute value.
         */
        void set_model(const std::string &model);

        /**
         * @brief Gets the model attribute that specifies the collection to iterate over.
         * @return The model attribute value.
         */
        const std::string &model() const;

        /**
         * @brief Clears the template nodes from the foreach node after rendering, to prevent them from being rendered again in subsequent iterations or when the document is rendered again.
         */
        void clear_template_nodes();

    private:
        std::vector<std::shared_ptr<DocraftNode> > template_nodes_;
        std::string model_;
    };
} // docraft

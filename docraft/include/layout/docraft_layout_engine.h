// C++
#pragma once

#include "docraft_lib.h"
#include <memory>
#include <vector>

#include "docraft_cursor.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_node.h"
#include "model/docraft_section.h"

namespace docraft::layout {
    namespace handler {
        class DocraftLayoutListHandler;
    }

    using Handlers = std::vector<std::unique_ptr<generic::DocraftChainOfResponsibilityHandler<model::DocraftNode, model::DocraftTransform>>>;
    /**
     * @brief Computes layout boxes for document nodes using a chain of handlers.
     *
     * The engine walks node trees and delegates box computation to specialized
     * handlers (text, table, list, etc.), maintaining a cursor for flow layout.
     */
    class DOCRAFT_LIB DocraftLayoutEngine {
    public:
        /**
         * @brief Creates a layout engine for the given document context.
         * @param context Document context used for measurements and page info.
         * @param reset_cursor Whether to reset the cursor before layout.
         */
        explicit DocraftLayoutEngine(const std::shared_ptr<DocraftDocumentContext>& context, bool reset_cursor = true);
        DocraftLayoutEngine(const DocraftLayoutEngine&) = delete;
        DocraftLayoutEngine& operator=(const DocraftLayoutEngine&) = delete;
        /**
         * @brief Destructor.
         */
        ~DocraftLayoutEngine() = default;

        /**
         * @brief Computes the layout for a single node tree.
         * @param node A shared pointer to a DocraftNode object.
         * @return A DocraftTransform representing the computed layout box.
         */
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode>& node);
        /**
         * @brief Computes the layout for a single node tree with a custom cursor.
         * @param node A shared pointer to a DocraftNode object.
         * @param cursor Cursor used for layout traversal.
         * @return A DocraftTransform representing the computed layout box.
         */
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode>& node, DocraftCursor& cursor);
        /**
         * @brief Computes the layout for a full document represented by a vector of nodes.
         * @param nodes A vector of shared pointers to DocraftNode objects.
         */
        void compute_document_layout(const std::vector<std::shared_ptr<model::DocraftNode>>& nodes);
    protected:
        /**
         * @brief Returns the bound document context.
         * @return Document context.
         */
        const std::shared_ptr<DocraftDocumentContext>& context() const;
        /**
         * @brief Computes the maximum rectangle that can contain all the given boxes.
         * @param boxes A vector of DocraftTransform representing the boxes.
         * @return A DocraftTransform representing the maximum rectangle.
         */
        static model::DocraftTransform compute_max_rect(const std::vector<model::DocraftTransform>& boxes) ;

    private:
        struct Sections {
            std::shared_ptr<model::DocraftHeader> header;
            std::shared_ptr<model::DocraftBody> body;
            std::shared_ptr<model::DocraftFooter> footer;
        };
        struct SectionPlan {
            bool header_to_render = false;
            bool body_to_render = false;
            bool footer_to_render = false;
            float header_ratio = 0.0F;
            float body_ratio = 0.0F;
            float footer_ratio = 0.0F;
        };

        /**
         * @brief Configures the handler chain for the current context.
         */
        void configure_handlers(const std::shared_ptr<DocraftDocumentContext>& context);
        Handlers handlers_;
        std::shared_ptr<DocraftDocumentContext> context_;
        const float kHeaderHeightRatio_ = 0.06F;
        const float kBodyHeightRatio_ = 0.88F;
        const float kFooterHeightRatio_ = 0.06F;
        const float kVerticalSpacing_ = 4.0F;
        const float kHorizontalSpacing_ = 4.0F;
        handler::DocraftLayoutListHandler* list_handler_ = nullptr;

        /**
         * @brief Execute the correct handler to compute the layout for the given node.
         * @param node
         * @param box Output transform.
         * @param cursor Cursor used for layout.
         * @return true if a handler processed the node.
         */
        bool compute_node(const std::shared_ptr<model::DocraftNode>& node, model::DocraftTransform* box, DocraftCursor& cursor) const;
        /**
         * @brief Computes the layout width available to a section node.
         * @param node Section node.
         * @return Available width in points.
         */
        float compute_width(const std::shared_ptr<model::DocraftSection> &node) const;
        /**
         * @brief Assigns page owner to a node and its children.
         * @param node Target node.
         * @param page Page number (1-based).
         */
        void assign_page_owner_recursive(const std::shared_ptr<model::DocraftNode>& node, int page) const;
        Sections split_sections(const std::vector<std::shared_ptr<model::DocraftNode>>& nodes) const;
        SectionPlan build_section_plan(const Sections& sections) const;
        void layout_header_section(const std::shared_ptr<model::DocraftHeader>& header, float header_ratio);
        void layout_body_section(const std::shared_ptr<model::DocraftBody>& body,
                                 const std::shared_ptr<model::DocraftHeader>& header,
                                 const SectionPlan& plan);
        void layout_footer_section(const std::shared_ptr<model::DocraftFooter>& footer,
                                   const std::shared_ptr<model::DocraftBody>& body,
                                   const SectionPlan& plan);
    };
} // layout

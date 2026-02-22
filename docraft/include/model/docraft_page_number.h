#pragma once

#include "docraft_lib.h"

#include "docraft_text.h"

namespace docraft::model {
    /**
     * @brief Text node that renders the current page number.
     *
     * The value is managed by the library and resolved from the document context
     * at layout/render time.
     */
    class DOCRAFT_LIB DocraftPageNumber : public DocraftText {
    public:
        using DocraftText::DocraftText;
        DocraftPageNumber();
        ~DocraftPageNumber() override = default;

        /**
         * @brief Updates the internal text to the current page number.
         * @param context Document context.
         */
        void update_text_from_context(const std::shared_ptr<DocraftDocumentContext>& context);
        /**
         * @brief Draws the page number using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext>& context) override;
        /**
         * @brief Clones the page number node and its line children.
         * @return Shared pointer to the cloned node.
         */
        std::shared_ptr<DocraftNode> clone() const override;
    };
} // docraft::model

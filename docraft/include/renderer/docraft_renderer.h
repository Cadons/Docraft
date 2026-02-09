#pragma once
#include <memory>

#include "model/docraft_blank_line.h"
#include "model/docraft_image.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_section.h"
#include "model/docraft_table.h"

namespace docraft::renderer {
    /*
     *@brief Abstract class that define the interface for rendering nodes
     **/
    class DocraftAbstractRenderer {
    public:
        explicit DocraftAbstractRenderer(const std::shared_ptr<DocraftDocumentContext> &docraftContext) : docraftContext_(
            docraftContext) {
        }

        virtual ~DocraftAbstractRenderer() = default;

        virtual void render_text(const model::DocraftText &text_node) = 0;

        virtual void render_section(const model::DocraftSection &section_node) = 0;

        virtual void render_image(const model::DocraftImage &image_node) = 0;

        virtual void render_table(const model::DocraftTable &table_node) = 0;

        virtual void render_rectangle(const model::DocraftRectangle &rectangle_node) = 0;

        virtual void render_blank_line(const model::DocraftBlankLine &blank_line_node)=0;

    protected:
        const std::shared_ptr<DocraftDocumentContext> &context() const {
            return docraftContext_;
        }

    private:
        std::shared_ptr<DocraftDocumentContext> docraftContext_;
    };
} // renderer

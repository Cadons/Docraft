#include "renderer/painter/docraft_blank_line_painter.h"

namespace docraft::renderer::painter {
    docraft_blank_line_painter::docraft_blank_line_painter(const model::DocraftBlankLine &blank_line_node) : blank_line_node_(
        blank_line_node) {
    }

    void docraft_blank_line_painter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        // Blank line does not render anything
    }
} // docraft

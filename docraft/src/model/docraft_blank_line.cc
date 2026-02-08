#include "model/docraft_blank_line.h"

#include <iostream>

#include "renderer/docraft_renderer.h"

namespace docraft::model {
    void DocraftBlankLine::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        // An empty line does not render anything, but we can log its presence if needed
        context->renderer()->render_blank_line(*this);
        // std::cout << "Drawing an empty line at position (" << x() << ", " << y() << ")\n";

    }
} // docraft
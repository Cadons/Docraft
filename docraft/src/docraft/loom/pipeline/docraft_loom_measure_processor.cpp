//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::pipeline {
    DocraftLoomMeasureProcessor::DocraftLoomMeasureProcessor(
        const std::shared_ptr<backend::IDocraftTextRenderingBackend>& text_backend)
        : text_backend_(text_backend)
    {
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomText* text)
    {
        if (text)
        {
            // Perform measurement logic for DocraftLoomText node
            auto text_width = text_backend_->measure_text_width(text->text());
            auto& measure_size = text->edit_layout_box().measured_size;

            measure_size.width = text_width;
            measure_size.height = text->font_size(); // Assuming font size is the height of the text
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* rectangle)
    {
        if (rectangle)
        {
            // Perform measurement logic for DocraftLoomRectangle node
        }
    }
} // docraft

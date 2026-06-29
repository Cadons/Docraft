//
// Created by Matteo on 29/06/2026.
//

#include "docraft/loom/docraft_loom_pdf_creator.h"

#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"

namespace docraft::loom {
    DocraftLoomPdfCreator::DocraftLoomPdfCreator(std::shared_ptr<interfaces::DocraftLoomIVisitorNode> root_node)
        : root_node_(std::move(root_node))
    {
        backend_ = std::make_shared<docraft::backend::pdf::DocraftHaruBackend>();
    }

    void DocraftLoomPdfCreator::create()
    {
        //run the pipeline
        auto text_backend = std::shared_ptr<backend::IDocraftTextRenderingBackend>(
            backend_, backend_->edit_text_rendering());
        auto measure_processor = pipeline::DocraftLoomMeasureProcessor(text_backend);
        auto layout_processor = pipeline::DocraftLoomLayoutProcessor();
        auto pagination_processor = pipeline::DocraftLoomPaginationProcessor();
        //run pipeline
        root_node_->accept(measure_processor);
        root_node_->accept(layout_processor);
        root_node_->accept(pagination_processor);
    }

    void DocraftLoomPdfCreator::render(const std::filesystem::path& output_path)
    {
        auto rendering_processor = pipeline::DocraftLoomRenderingProcessor(backend_->edit_text_rendering());
        //TODO:configure file render and save file
        root_node_->accept(rendering_processor);
        backend_->edit_output_backend()->save_to_file(output_path.string());
    }
} // docraft

//
// Created by Matteo on 29/06/2026.
//

#include "docraft/loom/docraft_loom_pdf_creator.h"

#include <cstddef>

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

    void DocraftLoomPdfCreator::set_header(std::shared_ptr<nodes::DocraftLoomNode> header)
    {
        header_ = std::move(header);
    }

    void DocraftLoomPdfCreator::set_footer(std::shared_ptr<nodes::DocraftLoomNode> footer)
    {
        footer_ = std::move(footer);
    }

    void DocraftLoomPdfCreator::set_section_ratios(float header_ratio, float body_ratio, float footer_ratio)
    {
        header_ratio_ = header_ratio;
        body_ratio_ = body_ratio;
        footer_ratio_ = footer_ratio;
    }

    void DocraftLoomPdfCreator::create()
    {
        //run the pipeline
        auto text_backend = std::shared_ptr<backend::IDocraftTextRenderingBackend>(
            backend_, backend_->edit_text_rendering());
        auto* page_backend = backend_->edit_page_rendering();
        const float page_width = page_backend->page_width();
        const float page_height = page_backend->page_height();

        const float header_height = page_height * header_ratio_;
        const float footer_height = page_height * footer_ratio_;
        body_top_y_ = header_height;
        body_height_ = page_height - header_height - footer_height;

        auto measure_processor = pipeline::DocraftLoomMeasureProcessor(text_backend);
        auto layout_processor = pipeline::DocraftLoomLayoutProcessor(page_width);

        // Header/footer are laid out once and stamped to render on every page -- the
        // real per-page split only applies to the body.
        if (header_)
        {
            header_->accept(measure_processor);
            layout_processor.reset_cursor(0.0F, 0.0F);
            header_->accept(layout_processor);
            pipeline::DocraftLoomPaginationProcessor::assign_page_index_recursive(*header_, -1);
        }
        if (footer_)
        {
            footer_->accept(measure_processor);
            layout_processor.reset_cursor(0.0F, page_height - footer_height);
            footer_->accept(layout_processor);
            pipeline::DocraftLoomPaginationProcessor::assign_page_index_recursive(*footer_, -1);
        }

        root_node_->accept(measure_processor);
        layout_processor.reset_cursor(0.0F, body_top_y_);
        root_node_->accept(layout_processor);

        auto* body_node = dynamic_cast<nodes::DocraftLoomNode*>(root_node_.get());
        auto pagination_processor = pipeline::DocraftLoomPaginationProcessor();
        total_page_count_ = body_node
                                ? pagination_processor.paginate_body(*body_node, body_top_y_, body_height_,
                                                                     page_backend)
                                : 1;
    }

    void DocraftLoomPdfCreator::render(const std::filesystem::path& output_path)
    {
        auto rendering_processor = pipeline::DocraftLoomRenderingProcessor(backend_.get());
        auto* page_backend = backend_->edit_page_rendering();

        page_backend->go_to_first_page();
        for (int page = 0; page < total_page_count_; ++page)
        {
            if (page > 0)
            {
                page_backend->go_to_page(static_cast<std::size_t>(page));
            }
            rendering_processor.set_current_page(page, total_page_count_);
            if (header_)
            {
                header_->accept(rendering_processor);
            }
            root_node_->accept(rendering_processor);
            if (footer_)
            {
                footer_->accept(rendering_processor);
            }
        }

        backend_->edit_output_backend()->save_to_file(output_path.string());
    }
} // docraft

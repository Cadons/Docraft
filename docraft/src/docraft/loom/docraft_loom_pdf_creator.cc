//
// Created by Matteo on 29/06/2026.
//

#include "docraft/loom/docraft_loom_pdf_creator.h"

#include <cstddef>

#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_font_registry.h"

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

    float DocraftLoomPdfCreator::header_ratio() const
    {
        return header_ratio_;
    }

    float DocraftLoomPdfCreator::body_ratio() const
    {
        return body_ratio_;
    }

    float DocraftLoomPdfCreator::footer_ratio() const
    {
        return footer_ratio_;
    }

    void DocraftLoomPdfCreator::set_page_format(docraft::backend::DocraftPageSize size,
                                                docraft::backend::DocraftPageOrientation orientation)
    {
        backend_->edit_page_rendering()->set_page_format(size, orientation);
    }

    void DocraftLoomPdfCreator::register_font(const std::string& family_name,
                                              const std::optional<std::string>& normal_path,
                                              const std::optional<std::string>& bold_path,
                                              const std::optional<std::string>& italic_path,
                                              const std::optional<std::string>& bold_italic_path)
    {
        auto* font_backend = backend_->edit_font_backend();
        auto register_variant = [&](const std::optional<std::string>& path, const std::string& alias_suffix)
        {
            if (!path)
            {
                return;
            }
            const char* internal_name = font_backend->register_ttf_font_from_file(*path, /*embed=*/true);
            if (!internal_name)
            {
                throw docraft::exception::BackendStateException("Failed to load font file: " + *path);
            }
            docraft::utils::DocraftFontRegistry::instance().register_font_alias(family_name + alias_suffix,
                internal_name);
        };
        register_variant(normal_path, "");
        register_variant(bold_path, "-Bold");
        register_variant(italic_path, "-Italic");
        register_variant(bold_italic_path, "-BoldItalic");
    }

    void DocraftLoomPdfCreator::set_metadata(const DocraftDocumentMetadata& metadata)
    {
        backend_->edit_metadata_backend()->set_document_metadata(metadata);
    }

    void DocraftLoomPdfCreator::set_header_margins(const Margins& margins)
    {
        header_margins_ = margins;
    }

    void DocraftLoomPdfCreator::set_body_margins(const Margins& margins)
    {
        body_margins_ = margins;
    }

    void DocraftLoomPdfCreator::set_footer_margins(const Margins& margins)
    {
        footer_margins_ = margins;
    }

    const std::shared_ptr<interfaces::DocraftLoomIVisitorNode>& DocraftLoomPdfCreator::root_node() const
    {
        return root_node_;
    }

    const std::shared_ptr<nodes::DocraftLoomNode>& DocraftLoomPdfCreator::header() const
    {
        return header_;
    }

    const std::shared_ptr<nodes::DocraftLoomNode>& DocraftLoomPdfCreator::footer() const
    {
        return footer_;
    }

    const DocraftLoomPdfCreator::Margins& DocraftLoomPdfCreator::header_margins() const
    {
        return header_margins_;
    }

    const DocraftLoomPdfCreator::Margins& DocraftLoomPdfCreator::body_margins() const
    {
        return body_margins_;
    }

    const DocraftLoomPdfCreator::Margins& DocraftLoomPdfCreator::footer_margins() const
    {
        return footer_margins_;
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
        body_top_y_ = header_height + body_margins_.top;
        body_height_ = page_height - header_height - footer_height - body_margins_.top - body_margins_.bottom;

        auto measure_processor = pipeline::DocraftLoomMeasureProcessor(text_backend);
        auto layout_processor = pipeline::DocraftLoomLayoutProcessor(page_width);

        // Header/footer are laid out once and stamped to render on every page -- the
        // real per-page split only applies to the body.
        if (header_)
        {
            measure_processor.set_content_width(page_width - header_margins_.left - header_margins_.right);
            header_->accept(measure_processor);
            layout_processor.set_content_width(page_width - header_margins_.left - header_margins_.right);
            layout_processor.reset_cursor(header_margins_.left, header_margins_.top);
            header_->accept(layout_processor);
            pipeline::DocraftLoomPaginationProcessor::assign_page_index_recursive(*header_, -1);
        }
        if (footer_)
        {
            measure_processor.set_content_width(page_width - footer_margins_.left - footer_margins_.right);
            footer_->accept(measure_processor);
            layout_processor.set_content_width(page_width - footer_margins_.left - footer_margins_.right);
            layout_processor.reset_cursor(footer_margins_.left,
                                          page_height - footer_height + footer_margins_.top);
            footer_->accept(layout_processor);
            pipeline::DocraftLoomPaginationProcessor::assign_page_index_recursive(*footer_, -1);
        }

        measure_processor.set_content_width(page_width - body_margins_.left - body_margins_.right);
        root_node_->accept(measure_processor);
        layout_processor.set_content_width(page_width - body_margins_.left - body_margins_.right);
        layout_processor.reset_cursor(body_margins_.left, body_top_y_);
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

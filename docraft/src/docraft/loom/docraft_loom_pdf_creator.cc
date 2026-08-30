//
// Created by Matteo on 29/06/2026.
//

#include "docraft/loom/docraft_loom_pdf_creator.h"

#include <algorithm>
#include <cstddef>

#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_layout_box_access.h"
#include "docraft/utils/docraft_logger.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"
#include "docraft/loom/pipeline/docraft_loom_pipeline_executor.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_font_registry.h"

namespace docraft::loom {
    DocraftLoomPdfCreator::DocraftLoomPdfCreator(std::shared_ptr<interfaces::DocraftLoomIVisitorNode> root_node)
        : root_node_(std::move(root_node))
    {
        backend_ = std::make_shared<docraft::backend::pdf::DocraftHaruBackend>();
        register_bundled_fonts();
    }

    void DocraftLoomPdfCreator::register_bundled_fonts()
    {
        auto* font_backend = backend_->edit_font_backend();
        auto& registry = docraft::utils::DocraftFontRegistry::instance();
        for (const auto& name : registry.raw_font_names())
        {
            const auto* font_data = registry.find_font(name);
            if (!font_data)
            {
                continue;
            }
            const char* internal_name = font_backend->register_ttf_font_from_memory(font_data->data,
                font_data->size, /*embed=*/true);
            if (!internal_name)
            {
                LOG_WARNING("Failed to embed bundled font '" + name + "'; it will not be available via font_name.");
                continue;
            }
            registry.register_font_alias(name, internal_name);
        }
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

        // header_ratio_/footer_ratio_ only set a *minimum* reserved space -- if a
        // region's actual content (now that padding()/margins genuinely inset it, see
        // DocraftLoomLayoutContainer::kDefaultPadding) needs more room than its ratio
        // allocates, that region grows instead of letting the body/footer overlap it.
        const float header_ratio_height = page_height * header_ratio_;
        const float footer_ratio_height = page_height * footer_ratio_;

        // Runs Measure -> Layout (and, for header/footer, the fixed page-index stamp)
        // for one region at a time, each with its own fresh pair of processor instances --
        // see DocraftLoomPipelineExecutor's class doc for why that matters (no
        // per-traversal state can leak between header/footer/body this way).
        const pipeline::DocraftLoomPipelineExecutor executor(text_backend, page_width);

        // Header/footer are laid out once and stamped to render on every page -- the
        // real per-page split only applies to the body.
        float header_extent = header_ratio_height;
        if (header_)
        {
            // Header's own top edge is always header_margins_.top from the page top --
            // unlike the footer, its position doesn't depend on its own height -- so it
            // can be measured and laid out in one pass, then extent read back after.
            executor.run(*header_, page_width - header_margins_.left - header_margins_.right,
                         header_margins_.left, header_margins_.top, /*assign_fixed_page_index=*/true);
            const float header_content_extent =
                header_margins_.top + nodes::sealed_frame(*header_).size.height + header_margins_.bottom;
            header_extent = std::max(header_extent, header_content_extent);
        }

        float footer_extent = footer_ratio_height;
        if (footer_)
        {
            // The footer is anchored to the page bottom, so its top position depends on
            // its own height -- measure it alone first (layout needs a resolved
            // position, which isn't known yet) to learn how much room it actually needs.
            auto footer_measure = pipeline::DocraftLoomMeasureProcessor(text_backend);
            footer_measure.set_content_width(page_width - footer_margins_.left - footer_margins_.right);
            footer_->accept(footer_measure);
            const float footer_content_extent = footer_->layout_box().measured_size.height
                + footer_margins_.top + footer_margins_.bottom;
            footer_extent = std::max(footer_extent, footer_content_extent);

            const float footer_top = page_height - footer_extent + footer_margins_.top;
            executor.run(*footer_, page_width - footer_margins_.left - footer_margins_.right,
                         footer_margins_.left, footer_top, /*assign_fixed_page_index=*/true);
        }

        body_top_y_ = header_extent + body_margins_.top;
        body_height_ = page_height - header_extent - footer_extent - body_margins_.top - body_margins_.bottom;

        executor.run(*root_node_, page_width - body_margins_.left - body_margins_.right, body_margins_.left,
                     body_top_y_);

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

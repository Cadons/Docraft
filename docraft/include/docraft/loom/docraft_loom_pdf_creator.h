//
// Created by Matteo on 29/06/2026.
//

#pragma once
#include <filesystem>
#include <memory>

#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "interfaces/docraft_loom_visitor.h"

namespace docraft::loom {
    /**
     * @brief Orchestrates the full loom pipeline (Measure -> Layout -> Pagination ->
     * Rendering) over an optional header/footer plus a body root, producing a
     * multi-page PDF. Header and footer are plain loom nodes (e.g. a VStack), not a
     * dedicated node type -- they are laid out once, stamped to render on every page,
     * and re-visited by the rendering pass for each physical page. The body is laid out
     * as one continuous flow and then split across pages by DocraftLoomPaginationProcessor.
     */
    class DocraftLoomPdfCreator
    {
    public:
        explicit DocraftLoomPdfCreator(std::shared_ptr<interfaces::DocraftLoomIVisitorNode> root_node);
        ~DocraftLoomPdfCreator() = default;

        /**
         * @brief Sets the header content, laid out once and re-drawn on every page.
         */
        void set_header(std::shared_ptr<nodes::DocraftLoomNode> header);
        /**
         * @brief Sets the footer content, laid out once and re-drawn on every page.
         */
        void set_footer(std::shared_ptr<nodes::DocraftLoomNode> footer);
        /**
         * @brief Sets the fraction of the page height given to header/body/footer.
         * Defaults to legacy's 0.06/0.88/0.06. Values are not required to sum to 1.0F,
         * but the body's own height is always page_height - header_height - footer_height
         * so it never overlaps the other two regions.
         */
        void set_section_ratios(float header_ratio, float body_ratio, float footer_ratio);

        /**
         * @brief Sets the header's margins (points), inset from the header region's own
         * top/bottom/left/right edges. Defaults to 0 on every side (today's behavior).
         */
        void set_header_margins(float top, float bottom, float left, float right);
        /**
         * @brief Sets the body's margins (points). The body's paginated flow starts
         * margin_top below the header and its per-page usable height shrinks by
         * margin_top + margin_bottom; margin_left/right inset every top-level child.
         */
        void set_body_margins(float top, float bottom, float left, float right);
        /**
         * @brief Sets the footer's margins (points), inset from the footer region's own
         * top/bottom/left/right edges. Defaults to 0 on every side (today's behavior).
         */
        void set_footer_margins(float top, float bottom, float left, float right);

        void create();
        void render(const std::filesystem::path& output_path);

    private:
        struct Margins
        {
            float top = 10.0F;
            float bottom = 10.0F;
            float left = 10.0F;
            float right = 10.0F;
        };

        std::shared_ptr<interfaces::DocraftLoomIVisitorNode> root_node_; //taken as input, the body
        std::shared_ptr<nodes::DocraftLoomNode> header_;
        std::shared_ptr<nodes::DocraftLoomNode> footer_;
        std::shared_ptr<docraft::backend::pdf::DocraftHaruSharedState> state_;
        std::shared_ptr<docraft::backend::pdf::DocraftHaruBackend> backend_; //used for text measurement and rendering

        float header_ratio_ = 0.06F;
        float body_ratio_ = 0.88F;
        float footer_ratio_ = 0.06F;
        Margins header_margins_;
        Margins body_margins_;
        Margins footer_margins_;
        float body_top_y_ = 0.0F;
        float body_height_ = 0.0F;
        int total_page_count_ = 1;
    };
} // docraft

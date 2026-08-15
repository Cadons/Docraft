//
// Created by Matteo on 29/06/2026.
//

#pragma once
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include "docraft/backend/docraft_page_format.h"
#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"
#include "docraft/docraft_document_metadata.h"
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
        /**
         * @brief Margins (points) inset from a section's (header/body/footer) own edges.
         */
        struct Margins
        {
            /**
             * @brief Default per-edge margin (points) applied when a section (or the
             * <Header>/<Body>/<Footer> element parsed from craft language) doesn't
             * override it via an explicit margin_top/margin_bottom/margin_left/
             * margin_right attribute -- keeps content from sitting flush against its
             * region's own edge.
             */
            static constexpr float kDefaultMarginPt = 20.0F;

            float top = kDefaultMarginPt;
            float bottom = kDefaultMarginPt;
            float left = kDefaultMarginPt;
            float right = kDefaultMarginPt;
        };

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
         * @brief Returns the header's fraction of page height (see set_section_ratios()).
         */
        float header_ratio() const;
        /**
         * @brief Returns the body's fraction of page height (see set_section_ratios()).
         */
        float body_ratio() const;
        /**
         * @brief Returns the footer's fraction of page height (see set_section_ratios()).
         */
        float footer_ratio() const;

        /**
         * @brief Sets the page size/orientation. Applies immediately to the backend, so
         * it must be called before create() -- create() reads the backend's page_width()/
         * page_height() to compute section heights.
         */
        void set_page_format(docraft::backend::DocraftPageSize size,
                             docraft::backend::DocraftPageOrientation orientation);

        /**
         * @brief Registers a font family from TTF file(s), so `font_name="family_name"`
         * (or `"family_name-Bold"`/`"-Italic"`/`"-BoldItalic"`) can be used by
         * `DocraftLoomText`. Each provided variant is loaded via the backend and aliased
         * in `DocraftFontRegistry` under `family_name` plus the matching suffix; a variant
         * left as `std::nullopt` is simply not registered (resolving that style falls back
         * to the closest available variant, or the requested name itself -- see
         * `DocraftFontResolver::resolve()`).
         * @throws docraft::exception::BackendStateException if a provided font file fails
         * to load.
         */
        void register_font(const std::string& family_name,
                           const std::optional<std::string>& normal_path,
                           const std::optional<std::string>& bold_path = std::nullopt,
                           const std::optional<std::string>& italic_path = std::nullopt,
                           const std::optional<std::string>& bold_italic_path = std::nullopt);

        /**
         * @brief Applies document info-dict metadata (title/author/etc.) to the backend.
         * Applies immediately; a later call overwrites what was set before.
         */
        void set_metadata(const DocraftDocumentMetadata& metadata);

        /**
         * @brief Sets the header's margins, inset from the header region's own edges.
         */
        void set_header_margins(const Margins& margins);
        /**
         * @brief Sets the body's margins. The body's paginated flow starts margin.top
         * below the header and its per-page usable height shrinks by
         * margin.top + margin.bottom; margin.left/right inset every top-level child.
         */
        void set_body_margins(const Margins& margins);
        /**
         * @brief Sets the footer's margins, inset from the footer region's own edges.
         */
        void set_footer_margins(const Margins& margins);

        void create();
        void render(const std::filesystem::path& output_path);

        /**
         * @brief Returns the body root node passed to the constructor (test/inspection
         * accessor -- mirrors DocraftLoomNode::edit_layout_box()'s non-const-getter
         * convention).
         */
        const std::shared_ptr<interfaces::DocraftLoomIVisitorNode>& root_node() const;
        /**
         * @brief Returns the header node, or nullptr if none was set.
         */
        const std::shared_ptr<nodes::DocraftLoomNode>& header() const;
        /**
         * @brief Returns the footer node, or nullptr if none was set.
         */
        const std::shared_ptr<nodes::DocraftLoomNode>& footer() const;
        const Margins& header_margins() const;
        const Margins& body_margins() const;
        const Margins& footer_margins() const;

    private:
        /**
         * @brief Embeds every bundled font (docraft/fonts.json, registered as raw bytes
         * into DocraftFontRegistry at static-init time) into this document's backend and
         * aliases it under its bundled name, so `font_name="Roboto"`/`"OpenSans"` etc.
         * work with no `<Settings><Fonts>` registration. Called once from the constructor,
         * before any document-declared `<Fonts>` are applied via register_font() -- a
         * document that registers its own font under the same family name simply
         * overwrites the bundled alias (register_font_alias() overwrites on duplicate key).
         * A bundled font that fails to embed is logged and skipped, not thrown -- unlike a
         * user's explicit, intentionally-declared font failing to load.
         */
        void register_bundled_fonts();

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

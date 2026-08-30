//
// Created by Matteo on 21/06/2026.
//

#pragma once
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include "docraft/docraft_lib.h"
#include "docraft/docraft_position.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"

namespace docraft::loom::nodes {
    /**
     * @brief Struct representing a size with width and height.
     */
    struct Size
    {
        float width;
        float height;
    };

    /**
     * @brief A position with x and y coordinates -- an alias for the shared
     * `docraft::Position`, so loom nodes, the cursor, and backend rendering
     * interfaces all share one point type.
     */
    using Position = docraft::Position;

    /**
     * @brief Struct representing a rectangle with a position and size.
     */
    struct Rect
    {
        Position position;
        Size size;
    };

    /**
     * @brief Positioning modes for loom layout.
     */
    enum class DocraftPositionType
    {
        kBlock,
        kAbsolute
    };

    /**
     * @brief Per-edge outer spacing (points), CSS-margin-shorthand style.
     */
    struct DocraftLoomMargin
    {
        float top = 0.0F;
        float right = 0.0F;
        float bottom = 0.0F;
        float left = 0.0F;
    };

    /**
     * @brief Struct representing a layout box, which encapsulates layout-related data for nodes.
     *
     * The `LayoutBox` struct stores the results of the pipeline's three sequential steps:
     * - Measurement step (`measured_size`): the calculated size of the node. Ungated --
     *   Measure is unconditionally the pipeline's first pass, so there's no "too early"
     *   caller to guard against.
     * - Layout step (`frame`): the final position and size to apply. Only readable/
     *   writable with a `LayoutProof`, mintable only by `DocraftLoomLayoutProcessor` --
     *   code from an earlier stage (Measure) simply cannot obtain one, so reading a
     *   frame before Layout has run is a compile error, not a silently wrong 0.
     * - Pagination step (`page_index`): which page this node landed on. Gated the same
     *   way by `PageIndexProof`, whose constructor additionally requires an existing
     *   `LayoutProof` -- so minting one is itself proof that Layout ran first, enforcing
     *   the Measure -> Layout -> Pagination order at the type level, not by convention.
     *
     * This header has no knowledge of the pipeline or of tests: `LayoutProof` and
     * `PageIndexProof` each name exactly one friend of their own --
     * `DocraftLoomLayoutBoxLayoutAccessor` and `DocraftLoomLayoutBoxPaginationAccessor`
     * respectively (both below) -- two narrow, protected-only minting seams, kept
     * separate so that inheriting the ability to seal a frame never also grants the
     * ability to seal a page index, or vice versa. Whoever legitimately needs to mint
     * one kind of proof -- `DocraftLoomLayoutProcessor` (frames),
     * `DocraftLoomPaginationProcessor` (page indices) in production,
     * `docraft::test::utils::LayoutBoxTestAccess` (both, for fixtures) in tests --
     * gets there by *inheriting* the matching accessor, not by being named here
     * individually; adding a new legitimate minter never requires touching this file
     * again.
     */
    class LayoutBox
    {
    public:
        class LayoutProof
        {
        public:
            LayoutProof(const LayoutProof&) = default;
            LayoutProof& operator=(const LayoutProof&) = default;

        private:
            friend class DocraftLoomLayoutBoxLayoutAccessor;
            LayoutProof() = default;
        };

        class PageIndexProof
        {
        public:
            PageIndexProof(const PageIndexProof&) = default;
            PageIndexProof& operator=(const PageIndexProof&) = default;

        private:
            friend class DocraftLoomLayoutBoxPaginationAccessor;
            explicit PageIndexProof(const LayoutProof&) {}
        };

        LayoutBox() = default;

        Size measured_size; // measure step result -- ungated, see class doc

        const Rect& frame(const LayoutProof&) const { return frame_; }
        Rect& edit_frame(const LayoutProof& proof) { layout_proof_ = proof; return frame_; }

        /**
         * @brief The proof Layout sealed onto this node, or nullopt if Layout hasn't
         * run for it yet -- the one runtime check in this scheme (whether Layout has
         * happened at all for this specific node), done once per read site instead of
         * trusted implicitly.
         */
        std::optional<LayoutProof> layout_proof() const { return layout_proof_; }

        int page_index(const PageIndexProof&) const { return page_index_; }
        void set_page_index(int index, const PageIndexProof& proof)
        {
            page_index_ = index;
            page_index_proof_ = proof;
        }
        std::optional<PageIndexProof> page_index_proof() const { return page_index_proof_; }

        /**
         * @brief Returns the sealed page index, or -1 (the existing "render on every
         * page" sentinel) if Pagination hasn't sealed one yet. Deliberately needs no
         * proof, unlike page_index(const PageIndexProof&): "not yet paginated" has
         * exactly one safe answer here (render everywhere), unlike frame()'s case,
         * where there is no safe placeholder geometry to fall back to -- so this
         * accessor can stay ungated without reintroducing the "silently wrong number"
         * failure mode this class exists to close. Used by
         * DocraftLoomRenderingProcessor::should_render(), which must keep working for
         * nodes/tests that never run real Pagination.
         */
        int page_index_or_unpaginated() const { return page_index_proof_ ? page_index_ : -1; }

    private:
        Rect frame_{};
        int page_index_ = -1;
        std::optional<LayoutProof> layout_proof_;
        std::optional<PageIndexProof> page_index_proof_;
    };

    /**
     * @brief The sole seam through which a `LayoutBox::LayoutProof` can be minted.
     * @details `DocraftLoomLayoutProcessor` inherits (`protected`) from this instead of
     * being named as a friend inside `LayoutBox::LayoutProof` itself, so this (model)
     * header never needs to know the pipeline's class names. Deliberately separate from
     * `DocraftLoomLayoutBoxPaginationAccessor`: a class that can seal a frame has no
     * business also being able to seal a page index just because both happened to
     * share one base.
     */
    class DOCRAFT_LIB DocraftLoomLayoutBoxLayoutAccessor
    {
    protected:
        static LayoutBox::LayoutProof make_layout_proof() { return LayoutBox::LayoutProof{}; }
    };

    /**
     * @brief The sole seam through which a `LayoutBox::PageIndexProof` can be minted.
     * @details `DocraftLoomPaginationProcessor` inherits (`protected`) from this
     * instead of being named as a friend inside `LayoutBox::PageIndexProof` itself.
     * Takes an existing `LayoutProof` (obtained via the public `LayoutBox::layout_proof()`
     * getter -- reading one requires no special access) rather than minting its own,
     * so inheriting this alone grants no ability to seal a frame.
     */
    class DOCRAFT_LIB DocraftLoomLayoutBoxPaginationAccessor
    {
    protected:
        static LayoutBox::PageIndexProof make_page_index_proof(const LayoutBox::LayoutProof& layout_proof)
        {
            return LayoutBox::PageIndexProof{layout_proof};
        }
    };

    /**
     * @brief Represents a node in the Docraft Loom structure, supporting child-node management
     *        and layout features.
     *
     * This class provides the ability to manage child nodes and store layout information through
     * the use of `LayoutBox`. It also inherits from `interfaces::DocraftLoomIVisitorNode`, enabling
     * visitor-based operations.
     */
    class DOCRAFT_LIB DocraftLoomNode : public interfaces::DocraftLoomIVisitorNode
    {
    public:
        using DocraftLoomIVisitorNode::DocraftLoomIVisitorNode;
        ~DocraftLoomNode() override = default;
        //children management
        int children_count() const;
        void add_child(std::shared_ptr<DocraftLoomNode> child);
        std::shared_ptr<DocraftLoomNode> remove_child(int index);
        std::shared_ptr<const DocraftLoomNode> child(int index) const;
        std::shared_ptr<DocraftLoomNode> edit_child(int index);
        void clear_children();
        /**
         * @brief Returns this node's direct-child indices (0..children_count()-1),
         * stable-sorted ascending by each child's z_index() -- the order in which
         * this node's own children should be painted (sibling-scoped only, CSS
         * stacking-context style; does not look into grandchildren or any other
         * container's children). Stability preserves declaration order among
         * children that share the same z_index.
         */
        std::vector<int> paint_order_indices() const;

        //layout box
        const LayoutBox& layout_box() const;
        LayoutBox& edit_layout_box();
        //absolute positioning
        /**
         * @brief Returns the positioning mode (block flow or absolute).
         */
        DocraftPositionType position_mode() const;
        /**
         * @brief Sets the positioning mode. Absolute nodes are placed at explicit_position()
         * during layout and do not advance the parent's cursor.
         */
        void set_position_mode(DocraftPositionType position_mode);
        /**
         * @brief Returns the explicit position used when position_mode() is kAbsolute.
         */
        const Position& explicit_position() const;
        /**
         * @brief Sets the explicit position used when position_mode() is kAbsolute.
         */
        void set_explicit_position(const Position& position);

        /**
         * @brief Returns the paint-order index. Higher values paint later (on top);
         * default 0 makes every existing node a no-op for z-index sorting.
         */
        int z_index() const;
        void set_z_index(int z_index);

        /**
         * @brief Returns the node's optional name (empty by default), settable from
         * Craft-language's `name` attribute for later lookup/debugging.
         */
        const std::string& name() const;
        void set_name(const std::string& name);

        /**
         * @brief Inset (in points, uniform on every side) between this node's own box
         * edge and where it lays out its children -- e.g. DocraftLoomRectangle offsets
         * its children's start position by padding() and grows its own measured size
         * by 2x padding() around them. A no-op default for node types that don't read
         * it (leaves like Text/Image/Circle).
         */
        float padding() const;
        void set_padding(float padding);

        /**
         * @brief Extra space (in points, per edge) this node asks its parent's
         * stacking layout to reserve around it, on top of whatever gap the parent's
         * own spacing()/spacing-like property already adds between children (see
         * DocraftLoomLayoutContainer::spacing()) -- e.g. a `<Title>` can set a larger
         * margin() than a plain `<Text>`/`<Paragraph>` so it reads as more separated
         * from what follows it, regardless of which container it's stacked in.
         * Containers combine the touching edges of two adjacent children via max(),
         * not sum (mirrors CSS margin collapsing), so two neighbors both asking for
         * breathing room don't double up -- vertical stacks (Rectangle/VStack) combine
         * bottom/top, horizontal stacks (HStack) combine right/left.
         */
        const DocraftLoomMargin& margin() const;
        void set_margin(float margin);
        void set_margin(float top, float right, float bottom, float left);

    private:
        std::vector<std::shared_ptr<DocraftLoomNode>> children_;
        LayoutBox layout_box_ = {};
        DocraftPositionType position_mode_ = DocraftPositionType::kBlock;
        Position explicit_position_{0.0F, 0.0F};
        int z_index_ = 0;
        std::string name_;
        float padding_ = 0.0F;
        DocraftLoomMargin margin_;
    };
} // docraft

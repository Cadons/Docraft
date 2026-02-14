#pragma once
#include <memory>

#include "docraft_cursor.h"
#include "backend/docraft_rendering_backend.h"
#include "generic/docraft_font_applier.h"

namespace docraft {
    namespace renderer {
        class DocraftAbstractRenderer;
    }
    namespace model {
        class DocraftHeader;
        class DocraftBody;
        class DocraftFooter;
    }

    /**
     * @brief Shared rendering and layout state for a document.
     *
     * The context holds the active rendering backend, cached backend interfaces,
     * page metrics, section nodes, and the layout cursor used by the engine.
     */
    class DocraftDocumentContext {
        public:
        /**
         * @brief Constructs a context with a default backend.
         */
        DocraftDocumentContext();
        /**
         * @brief Constructs a context with the provided rendering backend.
         * @param backend Rendering backend to use.
         */
        explicit DocraftDocumentContext(const std::shared_ptr<backend::IDocraftRenderingBackend>& backend);
        /**
         * @brief Releases context resources.
         */
        ~DocraftDocumentContext();

        /**
         * @brief Returns the active rendering backend.
         * @return Shared pointer to the rendering backend.
         */
        [[nodiscard]] const std::shared_ptr<backend::IDocraftRenderingBackend>& rendering_backend() const;
        /**
         * @brief Returns the mutable layout cursor.
         * @return Reference to the cursor.
         */
        DocraftCursor& cursor();
        /**
         * @brief Returns remaining vertical space on the current page section.
         * @return Available vertical space in points.
         */
        float available_space() const;
        /**
         * @brief Sets the renderer responsible for translating nodes to backend calls.
         * @param renderer Renderer instance.
         */
        void set_renderer(const std::shared_ptr<renderer::DocraftAbstractRenderer> &renderer);
        /**
         * @brief Returns the current renderer.
         * @return Shared pointer to the renderer (may be nullptr).
         */
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer();

        /**
         * @brief Sets the width of the current layout rectangle.
         * @param x Width in points.
         */
        void set_current_rect_width(float x);
        /**
         * @brief Returns the page width in points.
         * @return Page width in points.
         */
        [[nodiscard]] float page_width() const;
        /**
         * @brief Returns the page height in points.
         * @return Page height in points.
         */
        [[nodiscard]] float page_height() const;
        /**
         * @brief Sets the document header node.
         * @param header Header node.
         */
        void set_header(const std::shared_ptr<model::DocraftHeader> &header);
        /**
         * @brief Returns the header node.
         * @return Header node (may be nullptr).
         */
        [[nodiscard]] const std::shared_ptr<model::DocraftHeader>& header() const;
        /**
         * @brief Sets the document body node.
         * @param body Body node.
         */
        void set_body(const std::shared_ptr<model::DocraftBody> &body);
        /**
         * @brief Returns the body node.
         * @return Body node (may be nullptr).
         */
        [[nodiscard]] const std::shared_ptr<model::DocraftBody>& body() const;
        /**
         * @brief Sets the document footer node.
         * @param footer Footer node.
         */
        void set_footer(const std::shared_ptr<model::DocraftFooter> &footer);
        /**
         * @brief Returns the footer node.
         * @return Footer node (may be nullptr).
         */
        [[nodiscard]] const std::shared_ptr<model::DocraftFooter>& footer() const;
        /**
         * @brief Sets the font applier used for text nodes.
         * @param font_applier Font applier instance.
         */
        void set_font_applier(const std::shared_ptr<docraft::generic::DocraftFontApplier>& font_applier);
        /**
         * @brief Returns the font applier instance.
         * @return Font applier (may be nullptr).
         */
        [[nodiscard]] const std::shared_ptr<docraft::generic::DocraftFontApplier>& font_applier()const;
        /**
         * @brief Returns the line backend (cached).
         * @return Line rendering backend.
         */
        [[nodiscard]] const std::shared_ptr<backend::IDocraftLineRenderingBackend>& line_backend() const;
        /**
         * @brief Returns the shape backend (cached).
         * @return Shape rendering backend.
         */
        [[nodiscard]] const std::shared_ptr<backend::IDocraftShapeRenderingBackend>& shape_backend() const;
        /**
         * @brief Returns the text backend (cached).
         * @return Text rendering backend.
         */
        [[nodiscard]] const std::shared_ptr<backend::IDocraftTextRenderingBackend>& text_backend() const;
        /**
         * @brief Returns the image backend (cached).
         * @return Image rendering backend.
         */
        [[nodiscard]] const std::shared_ptr<backend::IDocraftImageRenderingBackend>& image_backend() const;
        /**
         * @brief Replaces the underlying rendering backend.
         * @param backend New rendering backend.
         */
        void set_backend(const std::shared_ptr<backend::IDocraftRenderingBackend>& backend);

    private:
        DocraftCursor cursor_;
        float current_rect_width_=0;
        std::shared_ptr<renderer::DocraftAbstractRenderer> renderer_;
        float page_width_;
        float page_height_;
        std::shared_ptr<model::DocraftHeader> header_;
        std::shared_ptr<model::DocraftBody> body_;
        std::shared_ptr<model::DocraftFooter> footer_;
        std::shared_ptr<docraft::generic::DocraftFontApplier> font_applier_;
        std::shared_ptr<backend::IDocraftRenderingBackend> backend_;
        mutable std::shared_ptr<backend::IDocraftLineRenderingBackend> line_backend_;
        mutable std::shared_ptr<backend::IDocraftShapeRenderingBackend> shape_backend_;
        mutable std::shared_ptr<backend::IDocraftTextRenderingBackend> text_backend_;
        mutable std::shared_ptr<backend::IDocraftImageRenderingBackend> image_backend_;
    };
} // docraft

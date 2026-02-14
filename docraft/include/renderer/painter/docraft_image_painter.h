#pragma once
#include "i_painter.h"
#include "model/docraft_image.h"

namespace docraft::renderer::painter {

    /**
     * @brief Painter that draws DocraftImage nodes.
     *
     * Supports file-based and in-memory image sources.
     */
    class DocraftImagePainter : public renderer::painter::IPainter{
        public:
        /**
         * @brief Creates an image painter bound to the image node.
         * @param image_node Image node.
         */
        explicit DocraftImagePainter(const model::DocraftImage &image_node);
        /**
         * @brief Draws the image using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

        private:
        const model::DocraftImage &image_node_;
    };
} // docraft

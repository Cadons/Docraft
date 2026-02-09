#include "renderer/painter/docraft_image_painter.h"

#include <stdexcept>

#include "backend/docraft_image_rendering_backend.h"

namespace docraft::renderer::painter {
    DocraftImagePainter::DocraftImagePainter(const model::DocraftImage &image_node) : image_node_(image_node) {
    }

    void DocraftImagePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) return;
        auto backend = context->image_backend();
        if (!backend) return;

        //start with pdf, then other formats will be supported
        switch (image_node_.format()) {
            case model::ImageFormat::kPng:
                backend->draw_png_image(
                    image_node_.path(),
                    image_node_.position().x,
                    image_node_.position().y - image_node_.height(),
                    image_node_.width(),
                    image_node_.height());
                break;
            case model::ImageFormat::kJpeg:
                backend->draw_jpeg_image(
                    image_node_.path(),
                    image_node_.position().x,
                    image_node_.position().y - image_node_.height(),
                    image_node_.width(),
                    image_node_.height());
                break;
            case model::ImageFormat::kRaw:
                if (image_node_.raw_data().empty()) {
                    throw std::runtime_error("Raw image data is empty");
                }
                backend->draw_raw_rgb_image_from_memory(
                    image_node_.raw_data().data(),
                    image_node_.raw_pixel_width(),
                    image_node_.raw_pixel_height(),
                    image_node_.position().x,
                    image_node_.position().y - image_node_.height(),
                    image_node_.width(),
                    image_node_.height());
                break;
//TODO: add raw in memory image in the templating user can load byte buffer with the image
            default:
                throw std::runtime_error("Unsupported image format");
        }
    }
} // docraft

#include "renderer/painter/docraft_image_painter.h"

#include <hpdf.h>
#include <hpdf_image.h>

namespace docraft::renderer::painter {
    DocraftImagePainter::DocraftImagePainter(const model::DocraftImage &image_node) : image_node_(image_node) {
    }

    void DocraftImagePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        auto *page = context->page();
        auto *pdf = context->pdf_doc();

        //start with pdf, then other formats will be supported
        HPDF_Image image;
        switch (image_node_.format()) {
            case model::ImageFormat::kPng:
                image = HPDF_LoadPngImageFromFile(pdf, image_node_.path().c_str());
                break;
            case model::ImageFormat::kJpeg:
                image = HPDF_LoadJpegImageFromFile(pdf, image_node_.path().c_str());
                break;
            case model::ImageFormat::kRaw:
                image = HPDF_LoadRawImageFromFile(pdf, image_node_.path().c_str(),
                                                  static_cast<HPDF_UINT>(image_node_.width()),
                                                  static_cast<HPDF_UINT>(image_node_.height()),
                                                  HPDF_CS_DEVICE_RGB);
                break;
//TODO: add raw in memory image in the templating user can load byte buffer with the image
            default:
                throw std::runtime_error("Unsupported image format");
        }


        HPDF_Page_DrawImage(page, image, image_node_.position().x, image_node_.position().y - image_node_.height(), image_node_.width(),
                            image_node_.height());
    }
} // docraft

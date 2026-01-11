#include "model/docraft_image.h"

#include <hpdf.h>

#include "renderer/docraft_renderer.h"

namespace docraft::model {
    DocraftImage::DocraftImage(){
        set_auto_fill_height(false);
        set_auto_fill_width(false);
    }

    DocraftImage::DocraftImage(DocraftImage *node) : DocraftNode(node) {
        path_ = node->path();
        format_ = node->format();
    }

    void DocraftImage::draw(const std::shared_ptr<DocraftPDFContext> &context) {
        context->renderer()->render_image(*this);
    }
    void DocraftImage::set_path(const std::string &path) {
        path_ = path;

        // Get file extension and determine format
        std::string extension = path_.substr(path_.find_last_of('.') + 1);
        switch (extension[0]) {
            case 'p':
            case 'P':
                format_ = ImageFormat::kPng;
                break;
            case 'j':
            case 'J':
                format_ = ImageFormat::kJpeg;
                break;
            default:
                format_ = ImageFormat::kRaw;
                break;
        }

        // Retrieve dimensions if not already set
        if (width() == 0 || height() == 0) {
            auto [orig_width, orig_height] = get_image_dimensions(path_, format_);
            set_width(orig_width);

            if (height() <= 0) {
                // Maintain aspect ratio
                float aspect_ratio = orig_height / orig_width;
                set_height(width() * aspect_ratio);
            }
        }
    }

    // Helper function to get image dimensions
    std::pair<float, float> DocraftImage::get_image_dimensions(const std::string &path, ImageFormat format) {
        HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
        HPDF_Image image = nullptr;

        if (format == ImageFormat::kPng) {
            image = HPDF_LoadPngImageFromFile(pdf, path.c_str());
        } else if (format == ImageFormat::kJpeg) {
            image = HPDF_LoadJpegImageFromFile(pdf, path.c_str());
        }

        float width = image ? HPDF_Image_GetWidth(image) : 0;
        float height = image ? HPDF_Image_GetHeight(image) : 0;

        HPDF_Free(pdf);
        return {width, height};
    }

    const std::string & DocraftImage::path() const {
        return path_;
    }
    ImageFormat DocraftImage::format() const {
        return format_;
    }
} // docraft
#include "model/docraft_image.h"

#include "renderer/docraft_renderer.h"

namespace docraft::model {
    DocraftImage::DocraftImage(){
        set_auto_fill_height(false);
        set_auto_fill_width(false);
    }

    DocraftImage::DocraftImage(DocraftImage *node) : DocraftNode(node) {
        path_ = node->path();
        format_ = node->format();
        raw_data_ = node->raw_data();
        raw_pixel_width_ = node->raw_pixel_width();
        raw_pixel_height_ = node->raw_pixel_height();
    }

    void DocraftImage::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
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
        //
        // // Retrieve dimensions if not already set
        // if (width() == 0 || height() == 0) {
        //     auto [orig_width, orig_height] = get_image_dimensions(path_, format_);
        //     set_width(orig_width);
        //
        //     if (height() <= 0) {
        //         // Maintain aspect ratio
        //         float aspect_ratio = orig_height / orig_width;
        //         set_height(width() * aspect_ratio);
        //     }
        // }
    }

    void DocraftImage::set_raw_data(const std::vector<unsigned char> &data, int pixel_width, int pixel_height) {
        raw_data_ = data;
        raw_pixel_width_ = pixel_width;
        raw_pixel_height_ = pixel_height;
        format_ = ImageFormat::kRaw;
    }

    // Helper function to get image dimensions
    std::pair<float, float> DocraftImage::get_image_dimensions(const std::string &path, ImageFormat format) {
        (void)path;
        (void)format;
        return {0.0F, 0.0F};
    }

    const std::string & DocraftImage::path() const {
        return path_;
    }
    ImageFormat DocraftImage::format() const {
        return format_;
    }
    const std::vector<unsigned char> &DocraftImage::raw_data() const {
        return raw_data_;
    }
    int DocraftImage::raw_pixel_width() const {
        return raw_pixel_width_;
    }
    int DocraftImage::raw_pixel_height() const {
        return raw_pixel_height_;
    }
} // docraft

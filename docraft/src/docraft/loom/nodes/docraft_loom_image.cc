#include "docraft/loom/nodes/docraft_loom_image.h"

namespace docraft::loom::nodes {
    void DocraftLoomImage::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const std::string& DocraftLoomImage::path() const
    {
        return path_;
    }

    void DocraftLoomImage::set_path(const std::string& path)
    {
        path_ = path;
        const std::string extension = path_.substr(path_.find_last_of('.') + 1);
        switch (extension.empty() ? '\0' : extension[0])
        {
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
    }

    ImageFormat DocraftLoomImage::format() const
    {
        return format_;
    }

    const std::vector<unsigned char>& DocraftLoomImage::raw_data() const
    {
        return raw_data_;
    }

    int DocraftLoomImage::raw_pixel_width() const
    {
        return raw_pixel_width_;
    }

    int DocraftLoomImage::raw_pixel_height() const
    {
        return raw_pixel_height_;
    }

    bool DocraftLoomImage::has_raw_data() const
    {
        return has_raw_data_;
    }

    void DocraftLoomImage::set_raw_data(const std::vector<unsigned char>& data, int pixel_width, int pixel_height)
    {
        raw_data_ = data;
        raw_pixel_width_ = pixel_width;
        raw_pixel_height_ = pixel_height;
        format_ = ImageFormat::kRaw;
        has_raw_data_ = true;
    }

    float DocraftLoomImage::width() const
    {
        return requested_width_;
    }

    void DocraftLoomImage::set_width(float width)
    {
        requested_width_ = width;
    }

    float DocraftLoomImage::height() const
    {
        return requested_height_;
    }

    void DocraftLoomImage::set_height(float height)
    {
        requested_height_ = height;
    }
} // docraft

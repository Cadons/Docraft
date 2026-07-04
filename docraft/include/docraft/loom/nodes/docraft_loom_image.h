#pragma once
#include <string>
#include <vector>

#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/model/docraft_image.h"

namespace docraft::loom::nodes {
    class DOCRAFT_LIB DocraftLoomImage : public DocraftLoomNode
    {
    public:
        DocraftLoomImage() = default;
        ~DocraftLoomImage() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const std::string& path() const;
        /**
         * @brief Sets the image path and derives format() from its file extension,
         * mirroring legacy's DocraftImage::set_path. Width/height are never auto-derived
         * from the file here (legacy's own dimension-detection is a stub returning {0,0}).
         */
        void set_path(const std::string& path);

        model::ImageFormat format() const;

        const std::vector<unsigned char>& raw_data() const;
        int raw_pixel_width() const;
        int raw_pixel_height() const;
        bool has_raw_data() const;
        void set_raw_data(const std::vector<unsigned char>& data, int pixel_width, int pixel_height);

        float width() const;
        void set_width(float width);
        float height() const;
        void set_height(float height);

    private:
        std::string path_;
        model::ImageFormat format_ = model::ImageFormat::kPng;
        std::vector<unsigned char> raw_data_;
        int raw_pixel_width_ = 0;
        int raw_pixel_height_ = 0;
        bool has_raw_data_ = false;
        float requested_width_ = 0.0F;
        float requested_height_ = 0.0F;
    };
} // docraft
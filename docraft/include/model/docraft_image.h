#pragma once
#include <vector>

#include "model/docraft_node.h"

namespace docraft::model {
        enum class ImageFormat{
                kPng,
                kJpeg,
                kRaw
            };
        class DocraftImage : public DocraftNode {
        public:
                DocraftImage();
                DocraftImage(const DocraftImage &node) = default;
                ~DocraftImage() override = default;
                explicit DocraftImage(DocraftImage *node);
                void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
                void set_path(const std::string &path);
                void set_raw_data(const std::vector<unsigned char> &data, int pixel_width, int pixel_height);

                static std::pair<float, float> get_image_dimensions(const std::string &path, ImageFormat format);

                [[nodiscard]] ImageFormat format() const;
                [[nodiscard]] const std::string &path() const;
                [[nodiscard]] const std::vector<unsigned char> &raw_data() const;
                [[nodiscard]] int raw_pixel_width() const;
                [[nodiscard]] int raw_pixel_height() const;

        private:
                std::string path_;
                ImageFormat format_=ImageFormat::kPng;
                std::vector<unsigned char> raw_data_;
                int raw_pixel_width_ = 0;
                int raw_pixel_height_ = 0;
        };
} // docraft

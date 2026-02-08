#pragma once
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

                static std::pair<float, float> get_image_dimensions(const std::string &path, ImageFormat format);

                [[nodiscard]] ImageFormat format() const;
                [[nodiscard]] const std::string &path() const;

        private:
                std::string path_;
                ImageFormat format_=ImageFormat::kPng;
        };
} // docraft
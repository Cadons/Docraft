#include <gtest/gtest.h>

#include <vector>

#include "docraft_document_context.h"
#include "model/docraft_image.h"
#include "renderer/painter/docraft_image_painter.h"

using namespace docraft;

TEST(DocraftImagePainter, DrawsRawImageFromMemory) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftImage image;
    image.set_position({.x = 10.0F, .y = 10.0F});
    image.set_width(1.0F);
    image.set_height(1.0F);
    std::vector<unsigned char> pixel = {255, 0, 0}; // 1x1 RGB
    image.set_raw_data(pixel, 1, 1);

    renderer::painter::DocraftImagePainter painter(image);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftImagePainter, ThrowsOnEmptyRawData) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftImage image;
    image.set_position({.x = 10.0F, .y = 10.0F});
    image.set_width(1.0F);
    image.set_height(1.0F);
    image.set_raw_data({}, 1, 1);

    renderer::painter::DocraftImagePainter painter(image);
    EXPECT_THROW(painter.draw(context), std::runtime_error);
}

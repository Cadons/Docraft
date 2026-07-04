#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

namespace docraft::test {
    class DocraftLoomImageTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            measure_ = std::make_unique<loom::pipeline::DocraftLoomMeasureProcessor>(nullptr);
            layout_ = std::make_unique<loom::pipeline::DocraftLoomLayoutProcessor>();
        }

        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> measure_;
        std::unique_ptr<loom::pipeline::DocraftLoomLayoutProcessor> layout_;
    };

    TEST_F(DocraftLoomImageTest, SetPathDerivesPngFormat)
    {
        loom::nodes::DocraftLoomImage image;
        image.set_path("photo.png");
        EXPECT_EQ(image.format(), model::ImageFormat::kPng);
    }

    TEST_F(DocraftLoomImageTest, SetPathDerivesJpegFormat)
    {
        loom::nodes::DocraftLoomImage image;
        image.set_path("photo.jpg");
        EXPECT_EQ(image.format(), model::ImageFormat::kJpeg);
    }

    TEST_F(DocraftLoomImageTest, SetPathDerivesRawFormatForUnknownExtension)
    {
        loom::nodes::DocraftLoomImage image;
        image.set_path("photo.bin");
        EXPECT_EQ(image.format(), model::ImageFormat::kRaw);
    }

    TEST_F(DocraftLoomImageTest, SetRawDataPopulatesHasRawData)
    {
        loom::nodes::DocraftLoomImage image;
        const std::vector<unsigned char> data = {1, 2, 3};
        image.set_raw_data(data, 1, 1);
        EXPECT_TRUE(image.has_raw_data());
        EXPECT_EQ(image.format(), model::ImageFormat::kRaw);
        EXPECT_EQ(image.raw_pixel_width(), 1);
        EXPECT_EQ(image.raw_pixel_height(), 1);
    }

    TEST_F(DocraftLoomImageTest, MeasureUsesExplicitWidthAndHeight)
    {
        loom::nodes::DocraftLoomImage image;
        image.set_width(120.0F);
        image.set_height(80.0F);
        image.accept(*measure_);

        EXPECT_FLOAT_EQ(image.layout_box().measured_size.width, 120.0F);
        EXPECT_FLOAT_EQ(image.layout_box().measured_size.height, 80.0F);
    }

    TEST_F(DocraftLoomImageTest, LayoutPlacesAtCursorAndAdvancesByHeight)
    {
        auto image = std::make_shared<loom::nodes::DocraftLoomImage>();
        image->set_width(50.0F);
        image->set_height(30.0F);
        image->accept(*measure_);
        image->accept(*layout_);

        EXPECT_FLOAT_EQ(image->layout_box().frame.size.width, 50.0F);
        EXPECT_FLOAT_EQ(image->layout_box().frame.size.height, 30.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomImage>();
        next->accept(*measure_);
        next->accept(*layout_);
        EXPECT_FLOAT_EQ(next->layout_box().frame.position.y,
                        image->layout_box().frame.position.y + 30.0F);
    }

    TEST_F(DocraftLoomImageTest, AbsolutePositionOverridesCursor)
    {
        loom::pipeline::DocraftLoomLayoutProcessor layout;
        auto image = std::make_shared<loom::nodes::DocraftLoomImage>();
        image->set_position_mode(model::DocraftPositionType::kAbsolute);
        image->set_explicit_position({15.0F, 25.0F});
        image->accept(*measure_);
        image->accept(layout);

        EXPECT_FLOAT_EQ(image->layout_box().frame.position.x, 15.0F);
        EXPECT_FLOAT_EQ(image->layout_box().frame.position.y, 25.0F);
    }
} // namespace docraft::test
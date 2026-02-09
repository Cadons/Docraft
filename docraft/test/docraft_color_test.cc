#include <gtest/gtest.h>
#include "docraft_color.h"

using namespace docraft;

TEST(DocraftColor, ConvertsKnownColorToRGB) {
    DocraftColor color(ColorName::kRed);
    RGB rgb = color.toRGB();
    EXPECT_FLOAT_EQ(rgb.r, 1.0F);
    EXPECT_FLOAT_EQ(rgb.g, 0.0F);
    EXPECT_FLOAT_EQ(rgb.b, 0.0F);
    EXPECT_FLOAT_EQ(rgb.a, 1.0F);
}

TEST(DocraftColor, ConvertsHexCodeToRGB) {
    DocraftColor color("#FF5733");
    RGB rgb = color.toRGB();
    EXPECT_FLOAT_EQ(rgb.r, 1.0F);
    EXPECT_NEAR(rgb.g, 87.0F / 255.0F, 1e-6F);
    EXPECT_NEAR(rgb.b, 51.0F / 255.0F, 1e-6F);
    EXPECT_FLOAT_EQ(rgb.a, 1.0F);
}

TEST(DocraftColor, ConvertsHexCodeWithAlphaToRGB) {
    DocraftColor color("#FF573380");
    RGB rgb = color.toRGB();
    EXPECT_NEAR(rgb.r, 1.0F, 1e-6F);
    EXPECT_NEAR(rgb.g, 87.0F / 255.0F, 1e-6F);
    EXPECT_NEAR(rgb.b, 51.0F / 255.0F, 1e-6F);
    EXPECT_NEAR(rgb.a, 128.0F / 255.0F, 1e-6F);
}

TEST(DocraftColor, HandlesInvalidHexCode) {
    DocraftColor color("invalid");
    RGB rgb = color.toRGB();
    EXPECT_FLOAT_EQ(rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(rgb.g, 0.0F);
    EXPECT_FLOAT_EQ(rgb.b, 0.0F);
    EXPECT_FLOAT_EQ(rgb.a, 1.0F);
}

TEST(DocraftColor, CreatesFromRGB) {
    DocraftColor color(0.5F, 0.5F, 0.5F, 0.5F);
    RGB rgb = color.toRGB();
    EXPECT_FLOAT_EQ(rgb.r, 0.5F);
    EXPECT_FLOAT_EQ(rgb.g, 0.5F);
    EXPECT_FLOAT_EQ(rgb.b, 0.5F);
    EXPECT_FLOAT_EQ(rgb.a, 0.5F);
}

TEST(DocraftColor, ConvertsToColorName) {
    DocraftColor color(ColorName::kBlue);
    EXPECT_EQ(color.toColorName(), ColorName::kBlue);
}

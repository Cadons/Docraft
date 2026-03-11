#include <gtest/gtest.h>

#include "docraft/model/docraft_settings.h"

namespace docraft::test::model {
    class DocraftSettingsTest : public ::testing::Test {
    protected:
        docraft::model::DocraftSettings settings;
    };

    TEST_F(DocraftSettingsTest, DefaultsAreA4PortraitAndDefaultRatios) {
        EXPECT_EQ(settings.page_size(), docraft::model::DocraftPageSize::kA4);
        EXPECT_EQ(settings.page_orientation(), docraft::model::DocraftPageOrientation::kPortrait);
        EXPECT_FALSE(settings.has_page_format());
        EXPECT_FALSE(settings.has_section_ratios());
        EXPECT_FLOAT_EQ(settings.header_ratio(), 0.06F);
        EXPECT_FLOAT_EQ(settings.body_ratio(), 0.88F);
        EXPECT_FLOAT_EQ(settings.footer_ratio(), 0.06F);
    }

    TEST_F(DocraftSettingsTest, CanSetPageFormat) {
        settings.set_page_format(docraft::model::DocraftPageSize::kA3,
                                 docraft::model::DocraftPageOrientation::kLandscape);
        EXPECT_TRUE(settings.has_page_format());
        EXPECT_EQ(settings.page_size(), docraft::model::DocraftPageSize::kA3);
        EXPECT_EQ(settings.page_orientation(), docraft::model::DocraftPageOrientation::kLandscape);
    }

    TEST_F(DocraftSettingsTest, CanSetSectionRatios) {
        settings.set_section_ratios(0.1F, 0.8F, 0.1F);
        EXPECT_TRUE(settings.has_section_ratios());
        EXPECT_FLOAT_EQ(settings.header_ratio(), 0.1F);
        EXPECT_FLOAT_EQ(settings.body_ratio(), 0.8F);
        EXPECT_FLOAT_EQ(settings.footer_ratio(), 0.1F);
    }
}

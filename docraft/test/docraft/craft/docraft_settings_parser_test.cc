#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/model/docraft_settings.h"

namespace docraft::test::craft {
    class DocraftSettingsParserTest : public ::testing::Test {
    protected:
        docraft::craft::parser::DocraftSettingsParser parser;
    };

    TEST_F(DocraftSettingsParserTest, ParsesPageFormatAndRatios) {
        const char *xml = R"XML(
<Settings page_size="A3" page_orientation="landscape" header_ratio="0.1" body_ratio="0.8" footer_ratio="0.1">
</Settings>
)XML";

        pugi::xml_document doc;
        ASSERT_TRUE(doc.load_string(xml));

        auto node = parser.parse(doc.child("Settings"));
        auto settings = std::dynamic_pointer_cast<docraft::model::DocraftSettings>(node);
        ASSERT_TRUE(settings);

        EXPECT_TRUE(settings->has_page_format());
        EXPECT_EQ(settings->page_size(), docraft::model::DocraftPageSize::kA3);
        EXPECT_EQ(settings->page_orientation(), docraft::model::DocraftPageOrientation::kLandscape);

        EXPECT_TRUE(settings->has_section_ratios());
        EXPECT_FLOAT_EQ(settings->header_ratio(), 0.1F);
        EXPECT_FLOAT_EQ(settings->body_ratio(), 0.8F);
        EXPECT_FLOAT_EQ(settings->footer_ratio(), 0.1F);
    }

    TEST_F(DocraftSettingsParserTest, ParsesOrientationWithoutSizeAsA4) {
        const char *xml = R"XML(
<Settings page_orientation="portrait" />
)XML";

        pugi::xml_document doc;
        ASSERT_TRUE(doc.load_string(xml));

        auto node = parser.parse(doc.child("Settings"));
        auto settings = std::dynamic_pointer_cast<docraft::model::DocraftSettings>(node);
        ASSERT_TRUE(settings);

        EXPECT_TRUE(settings->has_page_format());
        EXPECT_EQ(settings->page_size(), docraft::model::DocraftPageSize::kA4);
        EXPECT_EQ(settings->page_orientation(), docraft::model::DocraftPageOrientation::kPortrait);
    }

    TEST_F(DocraftSettingsParserTest, AppliesDefaultsForMissingRatios) {
        const char *xml = R"XML(
<Settings header_ratio="0.1" />
)XML";

        pugi::xml_document doc;
        ASSERT_TRUE(doc.load_string(xml));

        EXPECT_THROW(parser.parse(doc.child("Settings")), std::invalid_argument);
    }

    TEST_F(DocraftSettingsParserTest, RejectsInvalidRatios) {
        const char *xml = R"XML(
<Settings header_ratio="0.6" body_ratio="0.6" footer_ratio="0.1" />
)XML";

        pugi::xml_document doc;
        ASSERT_TRUE(doc.load_string(xml));

        EXPECT_THROW(parser.parse(doc.child("Settings")), std::invalid_argument);
    }

    TEST_F(DocraftSettingsParserTest, RejectsInvalidPageSize) {
        const char *xml = R"XML(
<Settings page_size="A0" />
)XML";

        pugi::xml_document doc;
        ASSERT_TRUE(doc.load_string(xml));

        EXPECT_THROW(parser.parse(doc.child("Settings")), std::invalid_argument);
    }

    TEST_F(DocraftSettingsParserTest, RejectsInvalidPageOrientation) {
        const char *xml = R"XML(
<Settings page_orientation="sideways" />
)XML";

        pugi::xml_document doc;
        ASSERT_TRUE(doc.load_string(xml));

        EXPECT_THROW(parser.parse(doc.child("Settings")), std::invalid_argument);
    }
}

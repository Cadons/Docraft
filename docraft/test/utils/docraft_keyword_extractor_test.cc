#include <gtest/gtest.h>

#include "docraft_document.h"
#include "model/docraft_text.h"
#include "utils/docraft_keyword_extractor.h"

namespace docraft::test::utils {
    TEST(DocraftKeywordExtractorTest, RanksKeywordsByFrequency) {
        DocraftDocument document("Keyword Stats");
        document.add_node(std::make_shared<model::DocraftText>(
            "analytics analytics analytics analytics parser parser parser metadata metadata rendering"));

        docraft::utils::DocraftKeywordExtractor extractor({
            .max_keywords = 3,
            .min_length = 4
        });

        const auto ranked = extractor.extract_with_frequency(document);
        ASSERT_EQ(ranked.size(), 3U);
        EXPECT_EQ(ranked[0].first, "analytics");
        EXPECT_EQ(ranked[0].second, 4U);
        EXPECT_EQ(ranked[1].first, "parser");
        EXPECT_EQ(ranked[1].second, 3U);
        EXPECT_EQ(ranked[2].first, "metadata");
        EXPECT_EQ(ranked[2].second, 2U);
    }

    TEST(DocraftKeywordExtractorTest, AppliesMinLengthAndStopWords) {
        DocraftDocument document("Keyword Filters");
        document.add_node(std::make_shared<model::DocraftText>(
            "delta delta gamma gamma gamma alpha alpha beta"));

        docraft::utils::DocraftKeywordExtractor extractor({
            .max_keywords = 5,
            .min_length = 5,
            .stop_words = {"delta"}
        });

        const auto ranked = extractor.extract_with_frequency(document);
        ASSERT_EQ(ranked.size(), 2U);
        EXPECT_EQ(ranked[0].first, "gamma");
        EXPECT_EQ(ranked[0].second, 3U);
        EXPECT_EQ(ranked[1].first, "alpha");
        EXPECT_EQ(ranked[1].second, 2U);
    }

    TEST(DocraftKeywordExtractorTest, UsesConfiguredLanguageStopWords) {
        DocraftDocument document("Keyword Languages");
        document.add_node(std::make_shared<model::DocraftText>(
            "el el el der der et et modelo analyse system"));

        docraft::utils::DocraftKeywordExtractor extractor({
            .max_keywords = 5,
            .min_length = 2,
            .stop_word_languages = {"es", "de", "fr"}
        });

        const auto ranked = extractor.extract(document);
        ASSERT_EQ(ranked.size(), 3U);
        EXPECT_EQ(ranked[0], "analyse");
        EXPECT_EQ(ranked[1], "modelo");
        EXPECT_EQ(ranked[2], "system");
    }
} // namespace docraft::test::utils

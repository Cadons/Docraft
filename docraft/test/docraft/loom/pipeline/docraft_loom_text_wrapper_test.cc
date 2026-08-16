#include "docraft/loom/pipeline/docraft_loom_text_wrapper.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "../../backend/docraft_mock_backend.h"

namespace docraft::test {
    class DocraftLoomTextWrapperTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            text_backend_mock_ = std::make_shared<::testing::NiceMock<backend::MockDocraftTextRenderingBackend>>();
            // Deterministic stand-in for real glyph measurement: one width unit per byte.
            ON_CALL(*text_backend_mock_, measure_text_width(::testing::_, ::testing::_, ::testing::_))
                .WillByDefault(
                    [](const std::string &text, const std::string &, float) { return static_cast<float>(text.size()); });
            wrapper_ = std::make_unique<loom::pipeline::DocraftLoomTextWrapper>(text_backend_mock_);
        }

        std::unique_ptr<loom::pipeline::DocraftLoomTextWrapper> wrapper_;
        std::shared_ptr<backend::MockDocraftTextRenderingBackend> text_backend_mock_;
    };

    // Regression test: wrapping a multi-word line whose full text is wider than
    // max_width must not reprint the boundary word -- each source word must appear
    // in the wrapped output exactly once, and every produced line must respect
    // max_width. Previously current_line.append(...).append(...) mutated
    // current_line as a side effect while building `candidate`, so the rejected
    // line got pushed already containing the overflowing word, which was then
    // placed again on the next line.
    TEST_F(DocraftLoomTextWrapperTest, MultiWordWrapDoesNotDuplicateBoundaryWord)
    {
        const auto lines = wrapper_->wrap("brown fox jumps", 9.0F, "Helvetica", 9.0F);

        EXPECT_THAT(lines, ::testing::ElementsAre("brown fox", "jumps"));
        for (const auto &line: lines) {
            EXPECT_LE(line.size(), 9U) << "line \"" << line << "\" exceeds max_width";
        }
    }

    TEST_F(DocraftLoomTextWrapperTest, MultiWordWrapPreservesEachWordExactlyOnce)
    {
        const auto lines = wrapper_->wrap("The quick brown fox jumps", 11.0F, "Helvetica", 9.0F);

        std::vector<std::string> words;
        for (const auto &line: lines) {
            std::istringstream iss(line);
            std::string word;
            while (iss >> word) {
                words.push_back(word);
            }
        }

        EXPECT_THAT(words, ::testing::ElementsAre("The", "quick", "brown", "fox", "jumps"));
    }

    TEST_F(DocraftLoomTextWrapperTest, SingleWordAloneStillCharacterSplitsCorrectly)
    {
        const auto lines = wrapper_->wrap("Balasubramanian", 6.0F, "Helvetica", 9.0F);

        std::string rejoined;
        for (const auto &line: lines) {
            rejoined += line;
        }
        EXPECT_EQ(rejoined, "Balasubramanian");
    }
} // namespace docraft::test

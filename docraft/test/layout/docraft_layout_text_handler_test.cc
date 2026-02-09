#include "layout/docraft_layout_engine.h"

#include <gtest/gtest.h>

#include "model/docraft_text.h"

namespace docraft::test::layout {
    class DocraftLayoutTextHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {
            context_ = std::make_shared<DocraftDocumentContext>();
            engine_ = std::make_unique<docraft::layout::DocraftLayoutEngine>(context_);
        }

        std::unique_ptr<docraft::layout::DocraftLayoutEngine>& engine() {
            return engine_;
        }

        std::shared_ptr<DocraftDocumentContext>& context() {
            return context_;
        }

    private:
        std::unique_ptr<docraft::layout::DocraftLayoutEngine> engine_;
        std::shared_ptr<DocraftDocumentContext> context_;
    };

    TEST_F(DocraftLayoutTextHandlerTest, TextNewlinesArePreservedAsSeparateLines) {
        auto text = std::make_shared<docraft::model::DocraftText>();
        text->set_text("Alpha\nBeta\nGamma");
        context()->set_current_rect_width(context()->page_width());

        engine()->compute_layout(text);

        const auto lines = text->lines();
        ASSERT_EQ(lines.size(), 3U);
        EXPECT_EQ(lines[0]->text(), "Alpha");
        EXPECT_EQ(lines[1]->text(), "Beta");
        EXPECT_EQ(lines[2]->text(), "Gamma");
    }

    TEST_F(DocraftLayoutTextHandlerTest, TextLinePositionsAreMonotonicNonIncreasing) {
        auto text = std::make_shared<docraft::model::DocraftText>();
        text->set_text("Line1\nLine2\nLine3\nLine4");
        context()->set_current_rect_width(context()->page_width());

        engine()->compute_layout(text);

        const auto lines = text->lines();
        ASSERT_EQ(lines.size(), 4U);
        for (std::size_t i = 1; i < lines.size(); ++i) {
            EXPECT_LE(lines[i]->position().y, lines[i - 1]->position().y);
        }
    }

    TEST_F(DocraftLayoutTextHandlerTest, JustifiedTextKeepsLastLineLeftAligned) {
        auto text = std::make_shared<docraft::model::DocraftText>();
        text->set_text("Uno due tre quattro cinque sei sette otto nove dieci");
        text->set_alignment(docraft::model::TextAlignment::kJustified);
        text->set_font_size(12.0F);
        context()->set_current_rect_width(80.0F);

        engine()->compute_layout(text);

        const auto lines = text->lines();
        ASSERT_GT(lines.size(), 1U);
        const auto &last_line = lines.back();
        EXPECT_EQ(last_line->alignment(), docraft::model::TextAlignment::kLeft);
        EXPECT_LT(last_line->width(), context()->available_space());
    }
} // namespace docraft::test::layout

#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../backend/docraft_mock_backend.h"

namespace docraft::test {
    class DocraftLoomMeasureProcessorTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            text_backend_mock_ = std::make_shared<::testing::NiceMock<backend::MockDocraftTextRenderingBackend>>();
            processor_ = std::make_unique<docraft::loom::pipeline::DocraftLoomMeasureProcessor>(text_backend_mock_);
        }

        void TearDown() override
        {
        }

        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor>& processor()
        {
            return processor_;
        }

        std::shared_ptr<backend::MockDocraftTextRenderingBackend>& text_backend_mock()
        {
            return text_backend_mock_;
        }

    private:
        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> processor_;
        std::shared_ptr<backend::MockDocraftTextRenderingBackend> text_backend_mock_;

        // Helper function to check if two floating-point numbers are approximately equal
        bool approximateEqual(float a, float b)
        {
            return fabs(a - b) < 1e-6;
        }
    };

    TEST_F(DocraftLoomMeasureProcessorTest, MeasureText)
    {
        auto text_node = std::make_unique<docraft::loom::nodes::DocraftLoomText>("Hello\nWorld");
        text_node->set_font_size(12.0F);
        EXPECT_CALL(*text_backend_mock(), measure_text_width("Hello\nWorld", ::testing::_, ::testing::_))
            .WillOnce(::testing::Return(11.0F));
        EXPECT_CALL(*text_backend_mock(), measure_text_height(::testing::_, 12.0F))
            .WillOnce(::testing::Return(12.0F));
        text_node->accept(*processor());

        EXPECT_EQ(text_node->edit_layout_box().measured_size.width, 11.0F);
        EXPECT_EQ(text_node->edit_layout_box().measured_size.height, 12.0F);
    }

    // --- Regression tests for confirmed-but-not-yet-fixed bugs from the code review
    // (.local/CODE_REVIEW_LOOM_MIGRATION.md). These currently FAIL against the
    // unmodified codebase -- they encode the desired behavior and will pass once the
    // corresponding fix is implemented.

    // Review bug #2 (Measure side): a weighted HStack nested inside a narrower
    // Rectangle must resolve its columns against the Rectangle's own width, not
    // the full page/region width.
    TEST_F(DocraftLoomMeasureProcessorTest, RectangleNarrowsWeightedHStackWidthInsteadOfFullRegion)
    {
        processor()->set_content_width(500.0F);

        auto rectangle = std::make_shared<docraft::loom::nodes::DocraftLoomRectangle>();
        rectangle->set_width(200.0F);
        rectangle->set_padding(0.0F); // isolate width-narrowing from the container's own default padding

        auto hstack = std::make_shared<docraft::loom::nodes::DocraftLoomHStack>();
        hstack->set_padding(0.0F); // isolate width-narrowing from the container's own default padding
        hstack->set_weights({1.0F, 1.0F});

        auto first = std::make_shared<docraft::loom::nodes::DocraftLoomText>("hello");
        auto second = std::make_shared<docraft::loom::nodes::DocraftLoomText>("world");
        hstack->add_child(first);
        hstack->add_child(second);
        rectangle->add_child(hstack);

        rectangle->accept(*processor());

        // Each column must be sized against the rectangle's 200pt budget (100 each),
        // not the full 500pt region width (which would give 250 each).
        EXPECT_NEAR(first->edit_layout_box().measured_size.width, 100.0F, 0.01F);
        EXPECT_NEAR(second->edit_layout_box().measured_size.width, 100.0F, 0.01F);
    }

    // Review bug #10: DocraftLoomPdfCreator::create() reuses a single
    // MeasureProcessor instance across header/footer/body via set_content_width(),
    // which only resets content_width_ -- inherited_wrap_width_ can leak from one
    // region into the next if the last node measured in a region doesn't consume
    // it (e.g. a Circle, which -- unlike Text -- never reads or clears it).
    TEST_F(DocraftLoomMeasureProcessorTest, WrapWidthDoesNotLeakBetweenRegions)
    {
        using ::testing::_;
        using ::testing::Return;
        EXPECT_CALL(*text_backend_mock(), measure_text_width(_, _, _)).WillRepeatedly(Return(42.0F));
        EXPECT_CALL(*text_backend_mock(), measure_text_height(_, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_mock(), measure_text_ascent(_, _)).WillRepeatedly(Return(8.0F));
        EXPECT_CALL(*text_backend_mock(), measure_text_descent(_, _)).WillRepeatedly(Return(-2.0F));

        // Header: a weighted HStack whose last column is a Circle -- it never reads
        // or clears inherited_wrap_width_, so the value pushed for it stays set.
        processor()->set_content_width(500.0F);
        auto header = std::make_shared<docraft::loom::nodes::DocraftLoomHStack>();
        header->set_weights({1.0F, 1.0F});
        header->add_child(std::make_shared<docraft::loom::nodes::DocraftLoomText>("h"));
        auto header_circle = std::make_shared<docraft::loom::nodes::DocraftLoomCircle>();
        header_circle->set_radius(5.0F);
        header->add_child(header_circle);
        header->accept(*processor());

        // Footer: mirrors DocraftLoomPdfCreator::create() calling set_content_width()
        // again before measuring the next region, then a bare Text with no explicit
        // wrap_width of its own.
        processor()->set_content_width(100.0F);
        auto footer_text = std::make_shared<docraft::loom::nodes::DocraftLoomText>("f");
        footer_text->accept(*processor());

        // With no leak, the footer's Text has no wrap constraint, so it measures its
        // own natural width via measure_text_width (mocked to 42.0F) -- not whatever
        // column width leaked from the header.
        EXPECT_FLOAT_EQ(footer_text->edit_layout_box().measured_size.width, 42.0F);
    }

    // The word-wrap char-split fallback (for a single word too wide to fit max_width
    // on its own) used to advance one byte at a time, which could split a multi-byte
    // UTF-8 character (e.g. an accented letter) in half. This word is five 2-byte
    // UTF-8 characters with no spaces -- "a e i o u" -- forcing that fallback path.
    TEST_F(DocraftLoomMeasureProcessorTest, WordWrapForceSplitDoesNotCorruptMultiByteUtf8Characters)
    {
        const std::string word = "\xC3\xA0\xC3\xA8\xC3\xAC\xC3\xB2\xC3\xB9"; // "àèìòù"
        auto text_node = std::make_unique<docraft::loom::nodes::DocraftLoomText>(word);
        text_node->set_font_size(12.0F);
        // 2.9, not a round 2.0/3.0: picked so the old byte-by-byte advance's greedy
        // growth stops right after accepting an *odd* byte offset (2.5 width, i.e. one
        // full character plus a lone lead byte of the next) instead of happening to
        // land back on a character boundary -- with a round width, both the buggy and
        // fixed advance produce the same (accidentally valid) split, and this test
        // would pass either way.
        text_node->set_wrap_width(2.9F);

        ON_CALL(*text_backend_mock(), measure_text_width(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Invoke([](const std::string& text, const std::string&, float)
            {
                // 1.0 width unit per UTF-8 codepoint (2 bytes each, here) -- a candidate
                // that split a codepoint in half would have an odd byte count, exactly
                // what the codepoint-aware advance must never produce.
                return static_cast<float>(text.size()) / 2.0F;
            }));
        ON_CALL(*text_backend_mock(), measure_text_height(::testing::_, ::testing::_)).WillByDefault(::testing::Return(12.0F));

        text_node->accept(*processor());

        std::string reassembled;
        for (const auto& line : text_node->wrapped_lines())
        {
            EXPECT_EQ(line.size() % 2, 0U) << "line \"" << line << "\" split a UTF-8 character in half";
            reassembled += line;
        }
        EXPECT_EQ(reassembled, word);
    }
}

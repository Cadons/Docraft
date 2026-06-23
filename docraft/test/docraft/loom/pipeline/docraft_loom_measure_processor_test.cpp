#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

#include "docraft/docraft_document_context.h"
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
            text_backend_mock_ = std::make_shared<backend::MockDocraftTextRenderingBackend>();
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
        EXPECT_CALL(*text_backend_mock(), measure_text_width("Hello\nWorld"))
            .WillOnce(::testing::Return(11.0F)); // Mock the width measurement
        text_node->accept(*processor()); //process node

        EXPECT_EQ(text_node->edit_layout_box().measured_size.width, 11.0F);
        EXPECT_EQ(text_node->edit_layout_box().measured_size.height, 12.0F);
    }
}

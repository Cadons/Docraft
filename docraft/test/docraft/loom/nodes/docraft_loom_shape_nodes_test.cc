#include <gtest/gtest.h>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

namespace docraft::test {
    class DocraftLoomShapeNodesTest : public ::testing::Test
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

    // ── Circle ──────────────────────────────────────────────────────────────────

    TEST_F(DocraftLoomShapeNodesTest, CircleMeasuresToDiameterFromRadius)
    {
        loom::nodes::DocraftLoomCircle circle;
        circle.set_radius(15.0F);
        circle.accept(*measure_);

        EXPECT_FLOAT_EQ(circle.layout_box().measured_size.width, 30.0F);
        EXPECT_FLOAT_EQ(circle.layout_box().measured_size.height, 30.0F);
    }

    TEST_F(DocraftLoomShapeNodesTest, CircleLayoutAdvancesCursorByDiameter)
    {
        auto circle = std::make_shared<loom::nodes::DocraftLoomCircle>();
        circle->set_radius(10.0F);
        circle->accept(*measure_);
        circle->accept(*layout_);

        EXPECT_FLOAT_EQ(circle->layout_box().frame.size.width, 20.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomCircle>();
        next->accept(*measure_);
        next->accept(*layout_);
        EXPECT_FLOAT_EQ(next->layout_box().frame.position.y, circle->layout_box().frame.position.y + 20.0F);
    }

    TEST_F(DocraftLoomShapeNodesTest, CircleStyleIsInheritedFromShapeBase)
    {
        loom::nodes::DocraftLoomCircle circle;
        circle.edit_style().border_width = 2.5F;
        EXPECT_FLOAT_EQ(circle.style().border_width, 2.5F);
    }

    // ── Triangle ─────────────────────────────────────────────────────────────────

    TEST_F(DocraftLoomShapeNodesTest, TriangleRejectsTwoPoints)
    {
        loom::nodes::DocraftLoomTriangle triangle;
        EXPECT_THROW(
            triangle.set_points({{.x = 0.0F, .y = 0.0F}, {.x = 10.0F, .y = 0.0F}}),
            exception::InvalidInputException);
    }

    TEST_F(DocraftLoomShapeNodesTest, TriangleRejectsFourPoints)
    {
        loom::nodes::DocraftLoomTriangle triangle;
        EXPECT_THROW(
            triangle.set_points({
                {.x = 0.0F, .y = 0.0F}, {.x = 10.0F, .y = 0.0F}, {.x = 5.0F, .y = 10.0F},
                {.x = 5.0F, .y = 5.0F}
            }),
            exception::InvalidInputException);
    }

    TEST_F(DocraftLoomShapeNodesTest, TriangleMeasuresBoundingBoxOfThreePoints)
    {
        loom::nodes::DocraftLoomTriangle triangle;
        triangle.set_points({{.x = 0.0F, .y = 0.0F}, {.x = 10.0F, .y = 0.0F}, {.x = 5.0F, .y = 10.0F}});
        triangle.accept(*measure_);

        EXPECT_FLOAT_EQ(triangle.layout_box().measured_size.width, 10.0F);
        EXPECT_FLOAT_EQ(triangle.layout_box().measured_size.height, 10.0F);
    }

    TEST_F(DocraftLoomShapeNodesTest, TriangleLayoutSetsFrameSizeToBoundingBox)
    {
        auto triangle = std::make_shared<loom::nodes::DocraftLoomTriangle>();
        triangle->set_points({{.x = 0.0F, .y = 0.0F}, {.x = 10.0F, .y = 0.0F}, {.x = 5.0F, .y = 10.0F}});
        triangle->accept(*measure_);
        triangle->accept(*layout_);

        EXPECT_FLOAT_EQ(triangle->layout_box().frame.size.width, 10.0F);
        EXPECT_FLOAT_EQ(triangle->layout_box().frame.size.height, 10.0F);
    }

    // ── Polygon ──────────────────────────────────────────────────────────────────

    TEST_F(DocraftLoomShapeNodesTest, PolygonAcceptsMoreThanThreePoints)
    {
        loom::nodes::DocraftLoomPolygon polygon;
        polygon.set_points({
            {.x = 0.0F, .y = 0.0F}, {.x = 10.0F, .y = 0.0F}, {.x = 10.0F, .y = 10.0F},
            {.x = 0.0F, .y = 10.0F}
        });
        EXPECT_EQ(polygon.points().size(), 4U);
    }

    TEST_F(DocraftLoomShapeNodesTest, PolygonMeasuresBoundingBoxOfFourPoints)
    {
        loom::nodes::DocraftLoomPolygon polygon;
        polygon.set_points({
            {.x = 0.0F, .y = 0.0F}, {.x = 20.0F, .y = 0.0F}, {.x = 20.0F, .y = 8.0F},
            {.x = 0.0F, .y = 8.0F}
        });
        polygon.accept(*measure_);

        EXPECT_FLOAT_EQ(polygon.layout_box().measured_size.width, 20.0F);
        EXPECT_FLOAT_EQ(polygon.layout_box().measured_size.height, 8.0F);
    }

    // ── Absolute positioning (one representative shape is enough per the plan) ────

    TEST_F(DocraftLoomShapeNodesTest, CircleAbsolutePositionOverridesCursor)
    {
        loom::nodes::DocraftLoomCircle circle;
        circle.set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        circle.set_explicit_position({33.0F, 44.0F});
        circle.set_radius(5.0F);
        circle.accept(*measure_);
        circle.accept(*layout_);

        EXPECT_FLOAT_EQ(circle.layout_box().frame.position.x, 33.0F);
        EXPECT_FLOAT_EQ(circle.layout_box().frame.position.y, 44.0F);
    }
} // namespace docraft::test
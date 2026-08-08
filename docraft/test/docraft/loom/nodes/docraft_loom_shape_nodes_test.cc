#include <gtest/gtest.h>

#include "docraft/docraft_color.h"
#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_curve_line.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

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

    TEST_F(DocraftLoomShapeNodesTest, PolygonAlwaysRendersAsAClosedShape)
    {
        // A polygon is only ever a polygon: the open-curve rendering it used to reach
        // through a smooth() flag now lives in its own node, DocraftLoomCurveLine.
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto polygon = std::make_shared<loom::nodes::DocraftLoomPolygon>();
        polygon->set_points({{.x = 0.0F, .y = 0.0F}, {.x = 10.0F, .y = 5.0F}, {.x = 20.0F, .y = 0.0F}});
        polygon->edit_style().border_color = DocraftColor::fromRGB(0.0F, 0.0F, 1.0F, 1.0F);
        polygon->edit_style().border_width = 2.0F;

        polygon->accept(*measure_);
        polygon->accept(*layout_);
        polygon->accept(rendering);

        EXPECT_TRUE(backend.draw_curve_calls().empty());
    }

    // ── CurveLine ───────────────────────────────────────────────────────────────

    TEST_F(DocraftLoomShapeNodesTest, CurveLineRendersAsCurveThroughItsPoints)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto curve = std::make_shared<loom::nodes::DocraftLoomCurveLine>();
        curve->set_points({{.x = 0.0F, .y = 0.0F}, {.x = 10.0F, .y = 5.0F}, {.x = 20.0F, .y = 0.0F}});
        curve->set_border_color(DocraftColor::fromRGB(0.0F, 0.0F, 1.0F, 1.0F));
        curve->set_border_width(2.0F);

        curve->accept(*measure_);
        curve->accept(*layout_);
        curve->accept(rendering);

        ASSERT_EQ(backend.draw_curve_calls().size(), 1U);
        EXPECT_EQ(backend.draw_curve_calls()[0].points.size(), 3U);
    }

    TEST_F(DocraftLoomShapeNodesTest, CurveLineMeasuresToTheBoundingBoxOfItsPoints)
    {
        loom::nodes::DocraftLoomCurveLine curve;
        curve.set_points({{.x = 0.0F, .y = 0.0F}, {.x = 30.0F, .y = 8.0F}, {.x = 12.0F, .y = 20.0F}});
        curve.accept(*measure_);

        EXPECT_FLOAT_EQ(curve.layout_box().measured_size.width, 30.0F);
        EXPECT_FLOAT_EQ(curve.layout_box().measured_size.height, 20.0F);
    }

    TEST_F(DocraftLoomShapeNodesTest, CurveLineWithTwoPointsDrawsASingleSegment)
    {
        // The interpolation degenerates to a straight line at 2 points, which is what
        // makes 2 a legal point count for a curve where a closed polygon needs 3.
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto curve = std::make_shared<loom::nodes::DocraftLoomCurveLine>();
        curve->set_points({{.x = 0.0F, .y = 0.0F}, {.x = 40.0F, .y = 10.0F}});

        curve->accept(*measure_);
        curve->accept(*layout_);
        curve->accept(rendering);

        ASSERT_EQ(backend.draw_curve_calls().size(), 1U);
        EXPECT_EQ(backend.draw_curve_calls()[0].points.size(), 2U);
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
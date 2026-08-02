#include <gtest/gtest.h>

#include "docraft/loom/charts/docraft_spline_chart_builder.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"

namespace docraft::test {
    using docraft::loom::charts::DocraftChartBuildContext;
    using docraft::loom::charts::DocraftChartSeries;
    using docraft::loom::charts::DocraftSplineChartBuilder;
    using docraft::loom::nodes::DocraftLoomCircle;
    using docraft::loom::nodes::DocraftLoomPolygon;

    namespace {
        template <typename NodeT>
        int count_children_of_type(const loom::nodes::DocraftLoomCanvas& canvas)
        {
            int count = 0;
            for (int i = 0; i < canvas.children_count(); ++i)
            {
                if (std::dynamic_pointer_cast<const NodeT>(canvas.child(i)))
                {
                    ++count;
                }
            }
            return count;
        }

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> build_spline_chart(const DocraftChartBuildContext& ctx)
        {
            return DocraftSplineChartBuilder{}.build(ctx);
        }
    } // namespace

    TEST(DocraftSplineChartBuilderTest, ReturnsCanvasWithRequestedDimensions)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        const auto canvas = build_spline_chart(ctx);
        ASSERT_TRUE(canvas);
        EXPECT_FLOAT_EQ(canvas->width(), 300.0F);
        EXPECT_FLOAT_EQ(canvas->height(), 200.0F);
    }

    TEST(DocraftSplineChartBuilderTest, DrawsOneSmoothCurvePerSeries)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.points = {{.x = 0.0F, .y = 1.0F}, {.x = 1.0F, .y = 2.0F}, {.x = 2.0F, .y = 1.0F}};
        DocraftChartSeries b;
        b.points = {{.x = 0.0F, .y = 3.0F}, {.x = 1.0F, .y = 1.0F}};
        ctx.series = {a, b};

        const auto canvas = build_spline_chart(ctx);
        int smooth_count = 0;
        for (int i = 0; i < canvas->children_count(); ++i)
        {
            if (auto polygon = std::dynamic_pointer_cast<const DocraftLoomPolygon>(canvas->child(i));
                polygon && polygon->smooth())
            {
                ++smooth_count;
            }
        }
        EXPECT_EQ(smooth_count, 2);
    }

    TEST(DocraftSplineChartBuilderTest, DrawsOneMarkerCirclePerDataPoint)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 1.0F}, {.x = 1.0F, .y = 2.0F}, {.x = 2.0F, .y = 1.0F}};
        ctx.series = {series};

        const auto canvas = build_spline_chart(ctx);
        EXPECT_EQ(count_children_of_type<DocraftLoomCircle>(*canvas), 3);
    }

    TEST(DocraftSplineChartBuilderTest, HandlesEmptySeriesWithoutCrashing)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_spline_chart(ctx));
        ASSERT_TRUE(canvas);
    }

    TEST(DocraftSplineChartBuilderTest, HandlesSinglePointSeriesWithoutCrashing)
    {
        // A curve needs >= 2 points to draw anything (add_curve() no-ops below that), but
        // a lone point should still get its marker so it isn't silently invisible.
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 1.0F}};
        ctx.series = {series};

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_spline_chart(ctx));
        EXPECT_EQ(count_children_of_type<DocraftLoomCircle>(*canvas), 1);
    }
} // namespace docraft::test

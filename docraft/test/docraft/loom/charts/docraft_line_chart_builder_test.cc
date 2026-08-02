#include <gtest/gtest.h>

#include "docraft/loom/charts/docraft_line_chart_builder.h"
#include "docraft/loom/charts/docraft_scatter_chart_builder.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_line.h"

namespace docraft::test {
    using docraft::loom::charts::DocraftChartBuildContext;
    using docraft::loom::charts::DocraftChartSeries;
    using docraft::loom::charts::DocraftLineChartBuilder;
    using docraft::loom::charts::DocraftScatterChartBuilder;
    using docraft::loom::nodes::DocraftLoomCircle;
    using docraft::loom::nodes::DocraftLoomLine;

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

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> build_line_chart(const DocraftChartBuildContext& ctx)
        {
            return DocraftLineChartBuilder{}.build(ctx);
        }
    } // namespace

    TEST(DocraftLineChartBuilderTest, ReturnsCanvasWithRequestedDimensions)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        const auto canvas = build_line_chart(ctx);
        ASSERT_TRUE(canvas);
        EXPECT_FLOAT_EQ(canvas->width(), 300.0F);
        EXPECT_FLOAT_EQ(canvas->height(), 200.0F);
    }

    TEST(DocraftLineChartBuilderTest, DrawsOneStraightSegmentPerConsecutivePointPairAcrossAllSeries)
    {
        // The chrome itself (gridlines/axes/ticks) also draws DocraftLoomLine nodes, so
        // isolate draw_series()'s own contribution by diffing against a scatter chart
        // built from the exact same ctx -- same data range means the same chrome tick
        // count (unlike diffing against an empty-series build, whose default [0,1]
        // data-space range would produce a different tick count), and scatter's own
        // draw_series() never adds a DocraftLoomLine.
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.points = {{.x = 0.0F, .y = 1.0F}, {.x = 1.0F, .y = 2.0F}, {.x = 2.0F, .y = 1.0F}};
        DocraftChartSeries b;
        b.points = {{.x = 0.0F, .y = 3.0F}, {.x = 1.0F, .y = 1.0F}};
        ctx.series = {a, b};

        const int chrome_only_lines = count_children_of_type<DocraftLoomLine>(*DocraftScatterChartBuilder{}.build(ctx));
        const auto canvas = build_line_chart(ctx);
        // a: 3 points -> 2 segments, b: 2 points -> 1 segment.
        constexpr int kExpectedSegments = 3;
        EXPECT_EQ(count_children_of_type<DocraftLoomLine>(*canvas), chrome_only_lines + kExpectedSegments);
    }

    TEST(DocraftLineChartBuilderTest, DrawsOneMarkerCirclePerDataPoint)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 1.0F}, {.x = 1.0F, .y = 2.0F}, {.x = 2.0F, .y = 1.0F}};
        ctx.series = {series};

        const auto canvas = build_line_chart(ctx);
        EXPECT_EQ(count_children_of_type<DocraftLoomCircle>(*canvas), 3);
    }

    TEST(DocraftLineChartBuilderTest, HandlesEmptySeriesWithoutCrashing)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_line_chart(ctx));
        ASSERT_TRUE(canvas);
    }

    TEST(DocraftLineChartBuilderTest, HandlesSinglePointSeriesWithoutCrashing)
    {
        // A segment needs >= 2 points to draw anything, but a lone point should still
        // get its marker so it isn't silently invisible.
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 1.0F}};
        ctx.series = {series};

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_line_chart(ctx));
        EXPECT_EQ(count_children_of_type<DocraftLoomCircle>(*canvas), 1);
    }
} // namespace docraft::test

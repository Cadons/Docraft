#include <gtest/gtest.h>

#include "docraft/loom/charts/docraft_pie_chart_builder.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::test {
    using docraft::loom::charts::DocraftChartBuildContext;
    using docraft::loom::charts::DocraftChartSeries;
    using docraft::loom::charts::DocraftPieChartBuilder;
    using docraft::loom::nodes::DocraftLoomPolygon;
    using docraft::loom::nodes::DocraftLoomText;

    namespace {
        int count_polygons(const loom::nodes::DocraftLoomCanvas& canvas)
        {
            int count = 0;
            for (int i = 0; i < canvas.children_count(); ++i)
            {
                if (std::dynamic_pointer_cast<const DocraftLoomPolygon>(canvas.child(i)))
                {
                    ++count;
                }
            }
            return count;
        }

        bool any_text_equals(const loom::nodes::DocraftLoomCanvas& canvas, const std::string& expected)
        {
            for (int i = 0; i < canvas.children_count(); ++i)
            {
                if (auto text = std::dynamic_pointer_cast<const DocraftLoomText>(canvas.child(i));
                    text && text->text() == expected)
                {
                    return true;
                }
            }
            return false;
        }

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> build_pie_chart(const DocraftChartBuildContext& ctx)
        {
            return DocraftPieChartBuilder{}.build(ctx);
        }
    } // namespace

    TEST(DocraftPieChartBuilderTest, ReturnsCanvasWithRequestedDimensions)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        const auto canvas = build_pie_chart(ctx);
        ASSERT_TRUE(canvas);
        EXPECT_FLOAT_EQ(canvas->width(), 300.0F);
        EXPECT_FLOAT_EQ(canvas->height(), 200.0F);
    }

    TEST(DocraftPieChartBuilderTest, DrawsOnePolygonSlicePerPositivePoint)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.name = "A";
        a.points = {{.x = 0.0F, .y = 10.0F}};
        DocraftChartSeries b;
        b.name = "B";
        b.points = {{.x = 0.0F, .y = 20.0F}};
        ctx.series = {a, b};

        const auto canvas = build_pie_chart(ctx);
        EXPECT_EQ(count_polygons(*canvas), 2);
        EXPECT_TRUE(any_text_equals(*canvas, "A"));
        EXPECT_TRUE(any_text_equals(*canvas, "B"));
    }

    TEST(DocraftPieChartBuilderTest, SkipsNonPositiveValuePoints)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.points = {{.x = 0.0F, .y = 10.0F}};
        DocraftChartSeries zero;
        zero.points = {{.x = 0.0F, .y = 0.0F}};
        DocraftChartSeries negative;
        negative.points = {{.x = 0.0F, .y = -5.0F}};
        ctx.series = {a, zero, negative};

        const auto canvas = build_pie_chart(ctx);
        EXPECT_EQ(count_polygons(*canvas), 1);
    }

    TEST(DocraftPieChartBuilderTest, TitleIsRenderedAsText)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        ctx.title = "Breakdown";
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 5.0F}};
        ctx.series = {series};

        const auto canvas = build_pie_chart(ctx);
        EXPECT_TRUE(any_text_equals(*canvas, "Breakdown"));
    }

    TEST(DocraftPieChartBuilderTest, HandlesEmptySeriesWithoutCrashing)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_pie_chart(ctx));
        ASSERT_TRUE(canvas);
        EXPECT_EQ(count_polygons(*canvas), 0);
    }

    TEST(DocraftPieChartBuilderTest, ShowsPercentageLabelsByDefault)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.points = {{.x = 0.0F, .y = 10.0F}};
        DocraftChartSeries b;
        b.points = {{.x = 0.0F, .y = 30.0F}};
        ctx.series = {a, b};

        const auto canvas = build_pie_chart(ctx);
        EXPECT_TRUE(any_text_equals(*canvas, "25%"));
        EXPECT_TRUE(any_text_equals(*canvas, "75%"));
    }

    TEST(DocraftPieChartBuilderTest, HidesPercentageLabelsWhenShowPercentageIsFalse)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        ctx.show_percentage = false;
        DocraftChartSeries a;
        a.points = {{.x = 0.0F, .y = 10.0F}};
        DocraftChartSeries b;
        b.points = {{.x = 0.0F, .y = 30.0F}};
        ctx.series = {a, b};

        const auto canvas = build_pie_chart(ctx);
        EXPECT_FALSE(any_text_equals(*canvas, "25%"));
        EXPECT_FALSE(any_text_equals(*canvas, "75%"));
    }

    TEST(DocraftPieChartBuilderTest, HandlesAllNonPositiveValuesWithoutCrashing)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 0.0F}, {.x = 1.0F, .y = -3.0F}};
        ctx.series = {series};

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_pie_chart(ctx));
        EXPECT_EQ(count_polygons(*canvas), 0);
    }
} // namespace docraft::test

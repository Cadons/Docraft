#include <gtest/gtest.h>

#include "docraft/loom/charts/docraft_histogram_chart_builder.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"

namespace docraft::test {
    using docraft::loom::charts::DocraftChartBuildContext;
    using docraft::loom::charts::DocraftChartSeries;
    using docraft::loom::charts::DocraftHistogramChartBuilder;
    using docraft::loom::nodes::DocraftLoomRectangle;

    namespace {
        int count_rectangles(const loom::nodes::DocraftLoomCanvas& canvas)
        {
            int count = 0;
            for (int i = 0; i < canvas.children_count(); ++i)
            {
                if (std::dynamic_pointer_cast<const DocraftLoomRectangle>(canvas.child(i)))
                {
                    ++count;
                }
            }
            return count;
        }

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> build_histogram_chart(const DocraftChartBuildContext& ctx)
        {
            return DocraftHistogramChartBuilder{}.build(ctx);
        }
    } // namespace

    TEST(DocraftHistogramChartBuilderTest, ReturnsCanvasWithRequestedDimensions)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        const auto canvas = build_histogram_chart(ctx);
        ASSERT_TRUE(canvas);
        EXPECT_FLOAT_EQ(canvas->width(), 300.0F);
        EXPECT_FLOAT_EQ(canvas->height(), 200.0F);
    }

    TEST(DocraftHistogramChartBuilderTest, DrawsOneBarPerDataPointAcrossAllSeriesPlusLegendSwatches)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.points = {{.x = 0.0F, .y = 3.0F}, {.x = 1.0F, .y = 5.0F}};
        DocraftChartSeries b;
        b.points = {{.x = 0.0F, .y = 2.0F}, {.x = 1.0F, .y = 4.0F}};
        ctx.series = {a, b};

        const auto canvas = build_histogram_chart(ctx);
        // 2 series x 2 points = 4 bars, plus a 2-row legend (2+ series always shows one)
        // -- both bars and legend swatches are DocraftLoomRectangle, so the total is 6.
        EXPECT_EQ(count_rectangles(*canvas), 6);
    }

    TEST(DocraftHistogramChartBuilderTest, BarsGrowFromZeroEvenWhenAllValuesAreFarFromIt)
    {
        // Regression test: bars used to grow from a baseline clamped to the auto-zoomed
        // data range, so a series entirely far from 0 (e.g. all values >= 5) rendered
        // bars starting from that range's own minimum instead of 0 -- making bar height
        // meaningless relative to the axis. adjust_data_bounds() now folds 0 into the
        // range before ticks are computed, so the baseline is always exactly 0.
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 10.0F}};
        ctx.series = {series};

        const auto canvas = build_histogram_chart(ctx);
        std::shared_ptr<const DocraftLoomRectangle> bar;
        for (int i = 0; i < canvas->children_count(); ++i)
        {
            if (auto rect = std::dynamic_pointer_cast<const DocraftLoomRectangle>(canvas->child(i)))
            {
                bar = rect;
                break;
            }
        }
        ASSERT_TRUE(bar);
        // A single-point series's own data range is a tiny sliver around its one value
        // (compute_data_bounds() widens a degenerate range by only +/-0.5). If the
        // baseline were still clamped into that sliver (the old bug), the bar would be a
        // few pixels tall; with 0 folded into the range, it spans most of the ~150px
        // plot height instead.
        EXPECT_GT(bar->height(), 50.0F);
    }

    TEST(DocraftHistogramChartBuilderTest, HandlesEmptySeriesWithoutCrashing)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_histogram_chart(ctx));
        ASSERT_TRUE(canvas);
    }
} // namespace docraft::test

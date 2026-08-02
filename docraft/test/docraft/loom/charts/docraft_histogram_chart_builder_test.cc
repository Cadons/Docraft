#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "docraft/loom/charts/docraft_histogram_chart_builder.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::test {
    using docraft::loom::charts::DocraftChartBuildContext;
    using docraft::loom::charts::DocraftChartSeries;
    using docraft::loom::charts::DocraftHistogramChartBuilder;
    using docraft::loom::nodes::DocraftLoomRectangle;
    using docraft::loom::nodes::DocraftLoomText;

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

    TEST(DocraftHistogramChartBuilderTest, LabeledPointsShowCategoryLabelAsXTickInsteadOfIndex)
    {
        // {"label": value} model entries (the shape pie/histogram model data always
        // uses) place a bar at their ordinal index, and format_x_tick_label() shows
        // their key as that bar's X-axis tick label instead of the raw numeric index.
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 3.0F}, {.x = 1.0F, .y = 5.0F}};
        series.point_labels = {"Jan", "Feb"};
        ctx.series = {series};

        const auto canvas = build_histogram_chart(ctx);
        EXPECT_TRUE(any_text_equals(*canvas, "Jan"));
        EXPECT_TRUE(any_text_equals(*canvas, "Feb"));
        EXPECT_FALSE(any_text_equals(*canvas, "0"));
        EXPECT_FALSE(any_text_equals(*canvas, "1"));
    }

    TEST(DocraftHistogramChartBuilderTest, YAxisAutoZoomsInsteadOfForcingBaselineToZero)
    {
        // The y-axis no longer forces 0 into its range (no adjust_data_bounds()
        // override) -- a series entirely far from 0 (900-1000, this test's own version
        // of the motivating example) gets an auto-zoomed tick range tight around its
        // real values ([900,1000] here -- nice-ticking lands exactly on the data since
        // both ends are already multiples of the chosen 20-unit step) instead of a
        // range stretched all the way down to real 0. The baseline (0) clamps to the
        // bottom of that tight range (900), so a point close to 900 -- still far from
        // real 0 -- renders a small bar; if 0 were still being forced into the range
        // (the old, now-removed behavior), that same point would render a bar spanning
        // most of the ~160px plot height instead.
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 900.0F}, {.x = 1.0F, .y = 920.0F}, {.x = 2.0F, .y = 1000.0F}};
        ctx.series = {series};

        const auto canvas = build_histogram_chart(ctx);
        std::vector<std::shared_ptr<const DocraftLoomRectangle>> bars;
        for (int i = 0; i < canvas->children_count(); ++i)
        {
            if (auto rect = std::dynamic_pointer_cast<const DocraftLoomRectangle>(canvas->child(i)))
            {
                bars.push_back(rect);
            }
        }
        ASSERT_EQ(bars.size(), 3U);
        // bars[1] is the y=920 point's bar -- close to the auto-zoomed baseline (900).
        EXPECT_LT(bars[1]->height(), 50.0F);
    }

    TEST(DocraftHistogramChartBuilderTest, BarLeavesAGapAtTheBaselineEdge)
    {
        // Same far-from-zero scenario as YAxisAutoZoomsInsteadOfForcingBaselineToZero --
        // the bar's bottom edge (where it meets the baseline) must sit strictly above
        // the plot's own bottom edge, proving a visible gap exists instead of the bar
        // touching the axis line. The plot's bottom edge is ctx.height minus the fixed
        // bottom-axis band (30px, no x_label here) reserved by the Cartesian chrome in
        // docraft_chart_builder.cc.
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 0.0F, .y = 1000.0F}};
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
        constexpr float kPlotBottomEdge = 200.0F - 30.0F;
        EXPECT_LT(bar->explicit_position().y + bar->height(), kPlotBottomEdge);
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

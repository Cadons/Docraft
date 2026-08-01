#include <gtest/gtest.h>

#include "docraft/loom/charts/docraft_chart_axis_helpers.h"

namespace docraft::test {
    using docraft::loom::charts::DataBounds;
    using docraft::loom::charts::DocraftChartAxisPosition;
    using docraft::loom::charts::DocraftChartSeries;
    using docraft::loom::charts::compute_axis_origin_data_space;
    using docraft::loom::charts::compute_data_bounds;
    using docraft::loom::charts::compute_nice_ticks;

    TEST(DocraftChartAxisHelpersTest, NiceTicksSnapToRoundStepsNotRawFractions)
    {
        const auto ticks = compute_nice_ticks(0.0F, 97.0F, 5);
        ASSERT_GE(ticks.size(), 2U);
        for (std::size_t i = 1; i < ticks.size(); ++i)
        {
            const float step = ticks[i] - ticks[i - 1];
            // A "nice" step is one of 1/2/5 * 10^n -- not an arbitrary fraction like
            // 97/4 = 24.25.
            EXPECT_TRUE(step == 20.0F || step == 25.0F || step == 10.0F || step == 5.0F)
                << "unexpected step " << step;
        }
        EXPECT_LE(ticks.front(), 0.0F);
        EXPECT_GE(ticks.back(), 97.0F);
    }

    TEST(DocraftChartAxisHelpersTest, NiceTicksHandlesDegenerateSingleValueRange)
    {
        EXPECT_NO_THROW({
            const auto ticks = compute_nice_ticks(5.0F, 5.0F, 5);
            EXPECT_GE(ticks.size(), 2U);
        });
    }

    TEST(DocraftChartAxisHelpersTest, DataBoundsCoversAllSeriesPoints)
    {
        std::vector<DocraftChartSeries> series(2);
        series[0].points = {{.x = 1.0F, .y = 5.0F}, {.x = 3.0F, .y = 1.0F}};
        series[1].points = {{.x = -2.0F, .y = 10.0F}};

        const DataBounds bounds = compute_data_bounds(series);
        EXPECT_FLOAT_EQ(bounds.min_x, -2.0F);
        EXPECT_FLOAT_EQ(bounds.max_x, 3.0F);
        EXPECT_FLOAT_EQ(bounds.min_y, 1.0F);
        EXPECT_FLOAT_EQ(bounds.max_y, 10.0F);
    }

    TEST(DocraftChartAxisHelpersTest, DataBoundsFallsBackToUnitRangeWhenEmpty)
    {
        const DataBounds bounds = compute_data_bounds({});
        EXPECT_FLOAT_EQ(bounds.min_x, 0.0F);
        EXPECT_FLOAT_EQ(bounds.max_x, 1.0F);
        EXPECT_FLOAT_EQ(bounds.min_y, 0.0F);
        EXPECT_FLOAT_EQ(bounds.max_y, 1.0F);
    }

    TEST(DocraftChartAxisHelpersTest, AxisOriginLeftIsBottomLeftCorner)
    {
        const DataBounds bounds{.min_x = 0.0F, .max_x = 10.0F, .min_y = 0.0F, .max_y = 20.0F};
        const auto origin = compute_axis_origin_data_space(DocraftChartAxisPosition::kLeft, bounds);
        EXPECT_FLOAT_EQ(origin.x, 0.0F);
        EXPECT_FLOAT_EQ(origin.y, 0.0F);
    }

    TEST(DocraftChartAxisHelpersTest, AxisOriginTopRightIsMaxXMaxY)
    {
        const DataBounds bounds{.min_x = 0.0F, .max_x = 10.0F, .min_y = 0.0F, .max_y = 20.0F};
        const auto origin = compute_axis_origin_data_space(DocraftChartAxisPosition::kTopRight, bounds);
        EXPECT_FLOAT_EQ(origin.x, 10.0F);
        EXPECT_FLOAT_EQ(origin.y, 20.0F);
    }

    TEST(DocraftChartAxisHelpersTest, AxisOriginCenterUsesZeroWhenWithinRange)
    {
        const DataBounds bounds{.min_x = -5.0F, .max_x = 5.0F, .min_y = -5.0F, .max_y = 5.0F};
        const auto origin = compute_axis_origin_data_space(DocraftChartAxisPosition::kCenter, bounds);
        EXPECT_FLOAT_EQ(origin.x, 0.0F);
        EXPECT_FLOAT_EQ(origin.y, 0.0F);
    }

    TEST(DocraftChartAxisHelpersTest, AxisOriginCenterClampsToMidpointWhenZeroOutOfRange)
    {
        const DataBounds bounds{.min_x = 10.0F, .max_x = 20.0F, .min_y = 10.0F, .max_y = 30.0F};
        const auto origin = compute_axis_origin_data_space(DocraftChartAxisPosition::kCenter, bounds);
        EXPECT_FLOAT_EQ(origin.x, 15.0F);
        EXPECT_FLOAT_EQ(origin.y, 20.0F);
    }
} // namespace docraft::test

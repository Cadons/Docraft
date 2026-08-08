#include <gtest/gtest.h>

#include <optional>

#include "docraft/loom/charts/docraft_scatter_chart_builder.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::test {
    using docraft::loom::charts::DocraftChartAxisPosition;
    using docraft::loom::charts::DocraftChartBuildContext;
    using docraft::loom::charts::DocraftChartSeries;
    using docraft::loom::charts::DocraftScatterChartBuilder;
    using docraft::loom::nodes::DocraftLoomCircle;
    using docraft::loom::nodes::DocraftLoomLine;
    using docraft::loom::nodes::DocraftLoomRectangle;
    using docraft::loom::nodes::DocraftLoomText;

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

        int index_of_first_child_of_type(const loom::nodes::DocraftLoomCanvas& canvas,
                                         const std::function<bool(const loom::nodes::DocraftLoomNode&)>& matches)
        {
            for (int i = 0; i < canvas.children_count(); ++i)
            {
                if (auto child = canvas.child(i); child && matches(*child))
                {
                    return i;
                }
            }
            return -1;
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

        std::optional<loom::nodes::Position> position_of_text(const loom::nodes::DocraftLoomCanvas& canvas,
                                                                const std::string& expected)
        {
            for (int i = 0; i < canvas.children_count(); ++i)
            {
                if (auto text = std::dynamic_pointer_cast<const DocraftLoomText>(canvas.child(i));
                    text && text->text() == expected)
                {
                    return text->explicit_position();
                }
            }
            return std::nullopt;
        }

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> build_scatter_chart(const DocraftChartBuildContext& ctx)
        {
            return DocraftScatterChartBuilder{}.build(ctx);
        }
    } // namespace

    TEST(DocraftScatterChartBuilderTest, ReturnsCanvasWithRequestedDimensions)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        const auto canvas = build_scatter_chart(ctx);
        ASSERT_TRUE(canvas);
        EXPECT_FLOAT_EQ(canvas->width(), 300.0F);
        EXPECT_FLOAT_EQ(canvas->height(), 200.0F);
    }

    TEST(DocraftScatterChartBuilderTest, DrawsOneCirclePerDataPointAcrossAllSeries)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.points = {{.x = 1.0F, .y = 1.0F}, {.x = 2.0F, .y = 2.0F}};
        DocraftChartSeries b;
        b.points = {{.x = 3.0F, .y = 3.0F}};
        ctx.series = {a, b};

        const auto canvas = build_scatter_chart(ctx);
        EXPECT_EQ(count_children_of_type<DocraftLoomCircle>(*canvas), 3);
    }

    TEST(DocraftScatterChartBuilderTest, PointsStayWithinCanvasBounds)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = -50.0F, .y = 5.0F}, {.x = 500.0F, .y = -5.0F}};
        ctx.series = {series};

        const auto canvas = build_scatter_chart(ctx);
        for (int i = 0; i < canvas->children_count(); ++i)
        {
            auto circle = std::dynamic_pointer_cast<const DocraftLoomCircle>(canvas->child(i));
            if (!circle)
            {
                continue;
            }
            EXPECT_GE(circle->explicit_position().x, -0.01F);
            EXPECT_LE(circle->explicit_position().x + circle->radius_x() * 2.0F, ctx.width + 0.01F);
            EXPECT_GE(circle->explicit_position().y, -0.01F);
            EXPECT_LE(circle->explicit_position().y + circle->radius_y() * 2.0F, ctx.height + 0.01F);
        }
    }

    TEST(DocraftScatterChartBuilderTest, NoLegendForSingleUnnamedSeries)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 1.0F, .y = 1.0F}};
        ctx.series = {series};

        const auto canvas = build_scatter_chart(ctx);
        EXPECT_EQ(count_children_of_type<DocraftLoomRectangle>(*canvas), 0);
    }

    TEST(DocraftScatterChartBuilderTest, LegendShownForNamedSingleSeries)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.name = "Revenue";
        series.points = {{.x = 1.0F, .y = 1.0F}};
        ctx.series = {series};

        const auto canvas = build_scatter_chart(ctx);
        EXPECT_EQ(count_children_of_type<DocraftLoomRectangle>(*canvas), 1);
        EXPECT_TRUE(any_text_equals(*canvas, "Revenue"));
    }

    TEST(DocraftScatterChartBuilderTest, LegendShownForMultipleUnnamedSeries)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries a;
        a.points = {{.x = 1.0F, .y = 1.0F}};
        DocraftChartSeries b;
        b.points = {{.x = 2.0F, .y = 2.0F}};
        ctx.series = {a, b};

        const auto canvas = build_scatter_chart(ctx);
        EXPECT_EQ(count_children_of_type<DocraftLoomRectangle>(*canvas), 2);
    }

    TEST(DocraftScatterChartBuilderTest, TitleCenteringUsesGlyphCountNotByteLength)
    {
        // Regression test: estimate_text_width() used to measure raw UTF-8 byte count
        // instead of codepoint count, so an accented title like "Citta" (5 codepoints, 6
        // bytes because of the 2-byte 'a') was overestimated and centered off from a
        // plain-ASCII title with the same glyph count.
        DocraftChartBuildContext ascii_ctx;
        ascii_ctx.width = 300.0F;
        ascii_ctx.height = 200.0F;
        ascii_ctx.title = "Citta";

        DocraftChartBuildContext accented_ctx;
        accented_ctx.width = 300.0F;
        accented_ctx.height = 200.0F;
        accented_ctx.title = "Citt\xC3\xA0"; // "Citt" + U+00E0 (a-grave), UTF-8 encoded

        const auto ascii_canvas = build_scatter_chart(ascii_ctx);
        const auto accented_canvas = build_scatter_chart(accented_ctx);

        const auto ascii_pos = position_of_text(*ascii_canvas, "Citta");
        const auto accented_pos = position_of_text(*accented_canvas, "Citt\xC3\xA0");
        ASSERT_TRUE(ascii_pos.has_value());
        ASSERT_TRUE(accented_pos.has_value());
        EXPECT_FLOAT_EQ(ascii_pos->x, accented_pos->x);
    }

    TEST(DocraftScatterChartBuilderTest, TitleAndAxisLabelsAreRenderedAsText)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        ctx.title = "Sales Overview";
        ctx.x_label = "Time";
        ctx.y_label = "Value";
        DocraftChartSeries series;
        series.points = {{.x = 1.0F, .y = 1.0F}};
        ctx.series = {series};

        const auto canvas = build_scatter_chart(ctx);
        EXPECT_TRUE(any_text_equals(*canvas, "Sales Overview"));
        EXPECT_TRUE(any_text_equals(*canvas, "Time"));
        EXPECT_TRUE(any_text_equals(*canvas, "Value"));
    }

    TEST(DocraftScatterChartBuilderTest, PaintOrderDrawsLinesBeforePoints)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;
        DocraftChartSeries series;
        series.points = {{.x = 1.0F, .y = 1.0F}};
        ctx.series = {series};

        const auto canvas = build_scatter_chart(ctx);
        const int first_line = index_of_first_child_of_type(
            *canvas, [](const loom::nodes::DocraftLoomNode& n)
            { return dynamic_cast<const DocraftLoomLine*>(&n) != nullptr; });
        const int first_circle = index_of_first_child_of_type(
            *canvas, [](const loom::nodes::DocraftLoomNode& n)
            { return dynamic_cast<const DocraftLoomCircle*>(&n) != nullptr; });

        ASSERT_NE(first_line, -1);
        ASSERT_NE(first_circle, -1);
        EXPECT_LT(first_line, first_circle);
    }

    TEST(DocraftScatterChartBuilderTest, HandlesEmptySeriesWithoutCrashing)
    {
        DocraftChartBuildContext ctx;
        ctx.width = 300.0F;
        ctx.height = 200.0F;

        std::shared_ptr<loom::nodes::DocraftLoomCanvas> canvas;
        EXPECT_NO_THROW(canvas = build_scatter_chart(ctx));
        ASSERT_TRUE(canvas);
        EXPECT_EQ(count_children_of_type<DocraftLoomCircle>(*canvas), 0);
    }
} // namespace docraft::test

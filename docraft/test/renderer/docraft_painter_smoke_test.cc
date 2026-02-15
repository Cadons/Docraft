#include <gtest/gtest.h>

#include "docraft_document_context.h"
#include "docraft_color.h"
#include "generic/docraft_font_applier.h"
#include "model/docraft_circle.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_table.h"
#include "model/docraft_text.h"
#include "model/docraft_triangle.h"
#include "model/docraft_line.h"
#include "model/docraft_polygon.h"
#include "renderer/painter/docraft_blank_line_painter.h"
#include "renderer/painter/docraft_circle_painter.h"
#include "renderer/painter/docraft_line_painter.h"
#include "renderer/painter/docraft_polygon_painter.h"
#include "renderer/painter/docraft_rectangle_painter.h"
#include "renderer/painter/docraft_table_painter.h"
#include "renderer/painter/docraft_text_painter.h"
#include "renderer/painter/docraft_triangle_painter.h"

using namespace docraft;

TEST(DocraftRectanglePainter, DrawsBasicRectangle) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftRectangle rect;
    rect.set_position({.x = 10.0F, .y = 10.0F});
    rect.set_width(50.0F);
    rect.set_height(20.0F);
    rect.set_background_color(DocraftColor(0.2F, 0.2F, 0.2F, 1.0F));
    rect.set_border_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));
    rect.set_border_width(1.0F);

    renderer::painter::DocraftRectanglePainter painter(rect);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftCirclePainter, DrawsBasicCircle) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftCircle circle;
    circle.set_position({.x = 50.0F, .y = 50.0F});
    circle.set_width(40.0F);
    circle.set_height(40.0F);
    circle.set_background_color(DocraftColor(0.2F, 0.2F, 0.8F, 1.0F));
    circle.set_border_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));
    circle.set_border_width(1.0F);

    renderer::painter::DocraftCirclePainter painter(circle);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftTrianglePainter, DrawsBasicTriangle) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftTriangle triangle;
    triangle.set_position({.x = 20.0F, .y = 80.0F});
    triangle.set_points({{.x = 0.0F, .y = 0.0F}, {.x = 40.0F, .y = 0.0F}, {.x = 20.0F, .y = 30.0F}});
    triangle.set_background_color(DocraftColor(0.8F, 0.2F, 0.2F, 1.0F));
    triangle.set_border_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));
    triangle.set_border_width(1.0F);

    renderer::painter::DocraftTrianglePainter painter(triangle);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftLinePainter, DrawsBasicLine) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftLine line;
    line.set_position({.x = 10.0F, .y = 10.0F});
    line.set_start({.x = 0.0F, .y = 0.0F});
    line.set_end({.x = 80.0F, .y = 20.0F});
    line.set_border_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));
    line.set_border_width(1.0F);

    renderer::painter::DocraftLinePainter painter(line);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftPolygonPainter, DrawsBasicPolygon) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftPolygon polygon;
    polygon.set_position({.x = 30.0F, .y = 90.0F});
    polygon.set_points({{.x = 0.0F, .y = 0.0F}, {.x = 30.0F, .y = 0.0F}, {.x = 40.0F, .y = 20.0F}, {.x = 10.0F, .y = 30.0F}});
    polygon.set_background_color(DocraftColor(0.2F, 0.8F, 0.2F, 1.0F));
    polygon.set_border_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));
    polygon.set_border_width(1.0F);

    renderer::painter::DocraftPolygonPainter painter(polygon);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftTextPainter, DrawsSingleLine) {
    auto context = std::make_shared<DocraftDocumentContext>();
    context->set_font_applier(std::make_shared<generic::DocraftFontApplier>(context));

    model::DocraftText text("Hello");
    text.set_font_name("Helvetica");
    text.set_font_size(12.0F);
    text.set_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));

    auto line = std::make_shared<model::DocraftText>("Hello");
    line->set_position({.x = 10.0F, .y = 20.0F});
    line->set_width(30.0F);
    line->set_height(12.0F);
    line->set_font_name("Helvetica");
    line->set_font_size(12.0F);
    line->set_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));
    text.add_line(line);

    renderer::painter::DocraftTextPainter painter(text);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftTablePainter, DrawsEmptyTable) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftTable table;
    table.set_position({.x = 10.0F, .y = 100.0F});
    table.set_width(100.0F);
    table.set_height(50.0F);
    table.set_rows(0);
    table.set_cols(0);

    renderer::painter::DocraftTablePainter painter(table);
    EXPECT_NO_THROW(painter.draw(context));
}

TEST(DocraftBlankLinePainter, DrawsNothing) {
    auto context = std::make_shared<DocraftDocumentContext>();

    model::DocraftBlankLine blank_line;
    renderer::painter::docraft_blank_line_painter painter(blank_line);
    EXPECT_NO_THROW(painter.draw(context));
}

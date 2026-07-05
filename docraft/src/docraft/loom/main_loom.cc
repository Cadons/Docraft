//
// Created by Matteo on 29/06/2026.
//
#include "docraft/loom/docraft_loom_foreach.h"
#include "docraft/loom/docraft_loom_pdf_creator.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace {
    using namespace docraft::loom::nodes;

    std::shared_ptr<DocraftLoomVStack> make_header_demo()
    {
        auto header = std::make_shared<DocraftLoomVStack>();
        header->set_spacing(4.0F);

        auto title = std::make_shared<DocraftLoomText>("Docraft Loom -- Annual Engineering Report");
        title->set_font_family("Helvetica");
        title->set_font_size(14.0F);
        title->set_bold(true);
        header->add_child(title);

        auto rule = std::make_shared<DocraftLoomLine>();
        rule->set_start({.x = 0.0F, .y = 0.0F});
        rule->set_end({.x = 555.0F, .y = 0.0F});
        rule->set_border_color(docraft::DocraftColor::fromRGB(0.6F, 0.6F, 0.6F, 1.0F));
        rule->set_border_width(1.0F);
        header->add_child(rule);

        return header;
    }

    std::shared_ptr<DocraftLoomHStack> make_footer_demo()
    {
        auto footer = std::make_shared<DocraftLoomHStack>();
        footer->set_spacing(4.0F);

        auto label = std::make_shared<DocraftLoomText>("Confidential -- Internal Use Only   |   Page");
        label->set_font_family("Helvetica");
        label->set_font_size(9.0F);
        footer->add_child(label);

        auto page_number = std::make_shared<DocraftLoomPageNumber>();
        page_number->set_font_family("Helvetica");
        page_number->set_font_size(9.0F);
        footer->add_child(page_number);

        return footer;
    }

    std::shared_ptr<DocraftLoomParagraph> make_paragraph(const std::string& title,
                                                         std::initializer_list<std::string> lines)
    {
        auto para = std::make_shared<DocraftLoomParagraph>();
        para->set_line_spacing(1.4F);
        para->set_space_before(5.0F);
        para->set_space_after(5.0F);
        auto heading = std::make_shared<DocraftLoomText>(title);
        heading->set_font_family("Helvetica");
        heading->set_font_size(14.0F);
        heading->set_bold(true);
        para->add_child(heading);
        for (const auto& line : lines)
        {
            auto t = std::make_shared<DocraftLoomText>(line);
            t->set_font_family("Helvetica");
            t->set_font_size(11.0F);
            para->add_child(t);
        }
        return para;
    }

    std::shared_ptr<DocraftLoomImage> make_image_demo()
    {
        // A tiny synthetic 2x2 red/blue checker, so the demo doesn't depend on any file
        // existing on disk.
        auto image = std::make_shared<DocraftLoomImage>();
        const std::vector<unsigned char> pixels = {
            255, 0, 0, 0, 0, 255, // row 0: red, blue
            0, 0, 255, 255, 0, 0, // row 1: blue, red
        };
        image->set_raw_data(pixels, 2, 2);
        image->set_width(40.0F);
        image->set_height(40.0F);
        return image;
    }

    std::shared_ptr<DocraftLoomRectangle> make_rectangle_with_children_demo()
    {
        auto rect = std::make_shared<DocraftLoomRectangle>();
        rect->set_padding(6.0F);
        rect->edit_style().background_color = docraft::DocraftColor::fromRGB(0.9F, 0.9F, 1.0F, 1.0F);
        rect->edit_style().border_color = docraft::DocraftColor::fromRGB(0.2F, 0.2F, 0.6F, 1.0F);
        rect->edit_style().border_width = 1.5F;
        auto label = std::make_shared<DocraftLoomText>("Rectangle wrapping a text child");
        rect->add_child(label);
        return rect;
    }

    std::shared_ptr<DocraftLoomRectangle> make_plain_rectangle_demo()
    {
        auto rect = std::make_shared<DocraftLoomRectangle>();
        rect->set_width(80.0F);
        rect->set_height(30.0F);
        rect->edit_style().background_color = docraft::DocraftColor::fromRGB(1.0F, 0.8F, 0.6F, 1.0F);
        rect->edit_style().border_width = 2.0F;
        return rect;
    }

    std::shared_ptr<DocraftLoomHStack> make_shapes_demo()
    {
        auto circle = std::make_shared<DocraftLoomCircle>();
        circle->set_radius(20.0F);
        circle->edit_style().background_color = docraft::DocraftColor::fromRGB(0.6F, 1.0F, 0.6F, 1.0F);

        auto triangle = std::make_shared<DocraftLoomTriangle>();
        triangle->set_points({{.x = 0.0F, .y = 0.0F}, {.x = 40.0F, .y = 0.0F}, {.x = 20.0F, .y = 40.0F}});
        triangle->edit_style().background_color = docraft::DocraftColor::fromRGB(1.0F, 1.0F, 0.6F, 1.0F);

        auto polygon = std::make_shared<DocraftLoomPolygon>();
        polygon->set_points({
            {.x = 0.0F, .y = 10.0F}, {.x = 20.0F, .y = 0.0F}, {.x = 40.0F, .y = 10.0F},
            {.x = 40.0F, .y = 30.0F}, {.x = 0.0F, .y = 30.0F}
        });
        polygon->edit_style().background_color = docraft::DocraftColor::fromRGB(1.0F, 0.6F, 1.0F, 1.0F);

        auto line = std::make_shared<DocraftLoomLine>();
        line->set_start({.x = 0.0F, .y = 0.0F});
        line->set_end({.x = 60.0F, .y = 0.0F});
        line->set_border_width(3.0F);

        auto row = std::make_shared<DocraftLoomHStack>();
        row->set_spacing(15.0F);
        row->add_child(circle);
        row->add_child(triangle);
        row->add_child(polygon);
        row->add_child(line);
        return row;
    }

    // Built with foreach_append: one list item per string, appended in order -- shows
    // Foreach used for the common "add_child per item" case.
    std::shared_ptr<DocraftLoomList> make_highlights_list_demo()
    {
        auto list = std::make_shared<DocraftLoomList>();
        list->set_kind(ListKind::kOrdered);
        list->set_ordered_style(OrderedListStyle::kNumber);

        const std::vector<std::string> highlights = {
            "Shipped the loom layout engine's core node types.",
            "Migrated table rendering to a unified grid model.",
            "Added real pagination with automatic table splitting.",
            "Introduced header/footer sections with page numbering.",
            "Reduced rendering coordinate bugs via consistent conventions.",
        };
        docraft::loom::foreach_append(*list, highlights,
                                      [](const std::string& text, int)
                                      {
                                          return std::make_shared<DocraftLoomText>(text);
                                      });

        return list;
    }

    std::shared_ptr<DocraftLoomList> make_box_marker_list_demo()
    {
        auto list = std::make_shared<DocraftLoomList>();
        list->set_kind(ListKind::kUnordered);
        list->set_unordered_dot(UnorderedListDot::kBox);
        list->add_child(std::make_shared<DocraftLoomText>("Finalize Q3 roadmap"));
        list->add_child(std::make_shared<DocraftLoomText>("Review pagination design doc"));
        return list;
    }

    std::shared_ptr<DocraftLoomTableCell> make_table_cell(const std::string& text, bool is_title = false)
    {
        auto cell = std::make_shared<DocraftLoomTableCell>();
        auto content = std::make_shared<DocraftLoomText>(text);
        if (is_title)
        {
            content->set_bold(true);
        }
        cell->set_content(content);
        cell->set_is_title(is_title);
        return cell;
    }

    // Built with foreach_item: table rows aren't appended via add_child (Table keeps its
    // own grid_), so each iteration calls add_row() directly instead -- shows Foreach
    // used for the "index-aware side-effecting loop" case. 100 rows comfortably overflow
    // a single page's body region, exercising DocraftLoomPaginationProcessor's table
    // splitting (the title row repeats on the continuation page).
    std::shared_ptr<DocraftLoomTable> make_team_roster_table_demo()
    {
        auto table = std::make_shared<DocraftLoomTable>();
        table->set_column_weights({0.4F, 0.3F, 0.3F});
        table->add_row({make_table_cell("Name", true), make_table_cell("Role", true), make_table_cell("Team", true)});

        const std::vector<std::string> roles = {"Engineer", "Designer", "Product Manager", "QA Analyst"};
        const std::vector<std::string> teams = {"Platform", "Growth", "Infrastructure", "Design Systems"};

        std::vector<int> employee_slots(100);
        docraft::loom::foreach_item(employee_slots, [&](int, int index)
        {
            const std::string name = "Employee " + std::to_string(index + 1);
            table->add_row({
                make_table_cell(name),
                make_table_cell(roles[static_cast<std::size_t>(index) % roles.size()]),
                make_table_cell(teams[static_cast<std::size_t>(index) % teams.size()]),
            });
        });

        return table;
    }

    // "Vertical"-style usage: column 0 holds a title cell per row (row labels), same
    // grid/algorithm as the roster table above -- just a different placement of
    // is_title() across the grid, demonstrating both table orientations with one code path.
    std::shared_ptr<DocraftLoomTable> make_profile_table_demo()
    {
        auto table = std::make_shared<DocraftLoomTable>();
        table->add_row({make_table_cell("Department", true), make_table_cell("Engineering")});
        table->add_row({make_table_cell("Headcount", true), make_table_cell("142")});
        table->add_row({make_table_cell("Location", true), make_table_cell("Remote-first")});
        return table;
    }

    // Two paragraphs side by side, each given weight 1 -- HStack::set_weights() divides
    // the available content width homogeneously between them (half the page each)
    // instead of each paragraph just shrinking to its own text width.
    std::shared_ptr<DocraftLoomHStack> make_side_by_side_paragraphs_demo()
    {
        auto row = std::make_shared<DocraftLoomHStack>();
        row->set_spacing(12.0F);
        row->set_weights({1.0F, 1.0F});

        row->add_child(make_paragraph("Roadmap", {
                                          "Next quarter focuses on richer text layout:",
                                          "wrapping, weighted rows and columns, and",
                                          "nested constraint-based sizing.",
                                      }));
        row->add_child(make_paragraph("Risks", {
                                          "Pagination edge cases for deeply nested",
                                          "containers still need broader test coverage",
                                          "before the legacy pipeline is retired.",
                                      }));
        return row;
    }

    std::shared_ptr<DocraftLoomRectangle> make_absolute_position_demo()
    {
        auto rect = std::make_shared<DocraftLoomRectangle>();
        rect->set_width(70.0F);
        rect->set_height(25.0F);
        rect->edit_style().background_color = docraft::DocraftColor::fromRGB(1.0F, 0.4F, 0.4F, 1.0F);
        rect->set_position_mode(docraft::model::DocraftPositionType::kAbsolute);
        // Placed away from the flowed content above so the demo clearly shows it
        // escaping the normal document flow rather than colliding with it.
        rect->set_explicit_position({.x = 420.0F, .y = 15.0F});
        return rect;
    }
} // namespace

int main()
{
    auto body = std::make_shared<docraft::loom::nodes::DocraftLoomVStack>();
    body->set_spacing(12.0F);

    body->add_child(make_paragraph("Overview", {
                                       "This report summarizes engineering progress on the Docraft Loom",
                                       "layout engine over the past quarter, including the components",
                                       "ported to it and the full multi-page rendering pipeline.",
                                   }));
    body->add_child(make_highlights_list_demo());
    body->add_child(std::make_shared<docraft::loom::nodes::DocraftLoomBlankLine>());

    body->add_child(make_paragraph("Visual Elements", {
                                       "Every basic node type below is exercised in this single document,",
                                       "including shapes, an embedded raster image and a bordered container.",
                                   }));
    body->add_child(make_shapes_demo());
    body->add_child(make_image_demo());
    body->add_child(make_rectangle_with_children_demo());
    body->add_child(make_plain_rectangle_demo());
    body->add_child(make_box_marker_list_demo());
    body->add_child(std::make_shared<docraft::loom::nodes::DocraftLoomBlankLine>());

    body->add_child(make_paragraph("Department Profile", {
                                       "A small table can also use a title column instead of a title",
                                       "row -- the same grid/algorithm handles both orientations.",
                                   }));
    body->add_child(make_profile_table_demo());
    body->add_child(std::make_shared<docraft::loom::nodes::DocraftLoomBlankLine>());

    body->add_child(make_paragraph("Team Roster", {
                                       "The table below spans multiple pages; its title row repeats",
                                       "automatically at the top of every continuation page.",
                                   }));
    body->add_child(make_team_roster_table_demo());
    body->add_child(std::make_shared<docraft::loom::nodes::DocraftLoomBlankLine>());

    body->add_child(make_side_by_side_paragraphs_demo());

    body->add_child(make_absolute_position_demo());

    docraft::loom::DocraftLoomPdfCreator creator(body);
    creator.set_header(make_header_demo());
    creator.set_footer(make_footer_demo());
    creator.create();
    creator.render("loom_output.pdf");
}

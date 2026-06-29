//
// Created by Matteo on 29/06/2026.
//
#include "docraft/loom/docraft_loom_pdf_creator.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

int main()
{
    auto make_paragraph = [](const std::string& title, std::initializer_list<std::string> lines)
    {
        auto para = std::make_shared<docraft::loom::nodes::DocraftLoomParagraph>();
        para->set_line_spacing(1.4F);
        para->set_space_before(5.0F);
        para->set_space_after(5.0F);
        auto heading = std::make_shared<docraft::loom::nodes::DocraftLoomText>(title);
        heading->set_font_family("Helvetica");
        heading->set_font_size(14.0F);
        para->add_child(heading);
        for (const auto& line : lines)
        {
            auto t = std::make_shared<docraft::loom::nodes::DocraftLoomText>(line);
            t->set_font_family("Helvetica");
            t->set_font_size(11.0F);
            para->add_child(t);
        }
        return para;
    };

    auto left_para = make_paragraph("Left Column", {
                                        "First line of the left paragraph.",
                                        "Second line with more content.",
                                        "Third line to fill the column."
                                    });

    auto right_para = make_paragraph("Right Column", {
                                         "First line of the right paragraph.",
                                         "Second line alongside the left.",
                                         "Third line in the right column."
                                     });

    auto hstack = std::make_shared<docraft::loom::nodes::DocraftLoomHStack>();
    hstack->set_spacing(20.0F);
    hstack->add_child(left_para);
    hstack->add_child(right_para);

    docraft::loom::DocraftLoomPdfCreator creator(hstack);
    creator.create();
    creator.render("loom_output.pdf");
}

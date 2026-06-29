#include "docraft/loom/docraft_loom_pdf_creator.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
//
// Created by Matteo on 29/06/2026.
//
int main()
{
    //create a test document with a text and a paragraphd
    std::shared_ptr<docraft::loom::nodes::DocraftLoomText> text_node = std::make_shared<
        docraft::loom::nodes::DocraftLoomText>("Hello, World!");
    text_node->set_font_family("Helvetica");
    text_node->set_font_size(12.0F);

    std::shared_ptr<docraft::loom::nodes::DocraftLoomParagraph> paragraph_node = std::make_shared<
        docraft::loom::nodes::DocraftLoomParagraph>();
    paragraph_node->set_line_spacing(1.2F);
    paragraph_node->set_space_before(10.0F);
    paragraph_node->set_space_after(10.0F);
    paragraph_node->add_child(std::make_shared<docraft::loom::nodes::DocraftLoomText>("This is a paragraph."));
    paragraph_node->
        add_child(std::make_shared<docraft::loom::nodes::DocraftLoomText>("It has multiple lines of text."));
    paragraph_node->add_child(
        std::make_shared<docraft::loom::nodes::DocraftLoomText>("Each line is a separate text node."));
    paragraph_node->add_child(
        std::make_shared<docraft::loom::nodes::DocraftLoomText>("The paragraph will be measured and laid out."));
    paragraph_node->add_child(
        std::make_shared<docraft::loom::nodes::DocraftLoomText>(
            "The layout processor will position each line correctly."));

    docraft::loom::DocraftLoomPdfCreator creator(paragraph_node);
    creator.create();
    creator.render("loom_output.pdf");
}

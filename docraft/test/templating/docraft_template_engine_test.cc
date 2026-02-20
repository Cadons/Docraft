#include <gtest/gtest.h>

#include "docraft_document.h"
#include "model/docraft_list.h"
#include "model/docraft_table.h"
#include "model/docraft_text.h"
#include "templating/docraft_template_engine.h"

namespace docraft::test::templating {
    class DocraftTemplateEngineTest : public ::testing::Test {
    protected:
        docraft::templating::DocraftTemplateEngine engine_;
    };

    TEST_F(DocraftTemplateEngineTest, AddsBase64ImageData) {
        // 2x1 RGB image: black then white -> bytes [0,0,0,255,255,255] => base64 "AAAA////"
        engine_.add_base64_image_data("test_image", "AAAA////", 2, 1);

        const auto &image = engine_.get_image_data("test_image");
        EXPECT_EQ(image.width, 2);
        EXPECT_EQ(image.height, 1);
        ASSERT_EQ(image.data.size(), 6U);
        EXPECT_EQ(image.data[0], 0);
        EXPECT_EQ(image.data[3], 255);
    }

    TEST_F(DocraftTemplateEngineTest, AddAndRetrieveVariable) {
        engine_.add_template_variable("title", "Docraft");
        EXPECT_TRUE(engine_.has_template_variable("title"));
        EXPECT_EQ(engine_.items(), 1);
        EXPECT_EQ(engine_.get_template_variable("title"), "Docraft");
    }

    TEST_F(DocraftTemplateEngineTest, AddDuplicateVariableThrows) {
        engine_.add_template_variable("title", "Docraft");
        EXPECT_THROW(engine_.add_template_variable("title", "Other"), std::runtime_error);
        EXPECT_EQ(engine_.items(), 1);
        EXPECT_EQ(engine_.get_template_variable("title"), "Docraft");
    }

    TEST_F(DocraftTemplateEngineTest, GetMissingVariableThrows) {
        EXPECT_THROW(engine_.get_template_variable("missing"), std::runtime_error);
    }

    TEST_F(DocraftTemplateEngineTest, RemoveVariable) {
        engine_.add_template_variable("author", "Matteo");
        EXPECT_TRUE(engine_.has_template_variable("author"));
        engine_.remove_template_variable("author");
        EXPECT_FALSE(engine_.has_template_variable("author"));
        EXPECT_EQ(engine_.items(), 0);
    }

    TEST_F(DocraftTemplateEngineTest, RemoveMissingVariableThrows) {
        EXPECT_THROW(engine_.remove_template_variable("missing"), std::runtime_error);
    }

    TEST_F(DocraftTemplateEngineTest, ClearVariables) {
        engine_.add_template_variable("title", "Docraft");
        engine_.add_template_variable("author", "Matteo");
        EXPECT_EQ(engine_.items(), 2);
        engine_.clear_template_variables();
        EXPECT_EQ(engine_.items(), 0);
        EXPECT_FALSE(engine_.has_template_variable("title"));
        EXPECT_FALSE(engine_.has_template_variable("author"));
    }

    TEST_F(DocraftTemplateEngineTest, TemplateAppliesToTableNodes) {
        docraft::DocraftDocument document("Test Document");
        auto template_engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
        template_engine->add_template_variable("title", "Docraft");
        template_engine->add_template_variable("value", "42");
        document.set_document_template_engine(template_engine);

        auto table = std::make_shared<model::DocraftTable>();
        table->set_cols(1);
        table->set_rows(1);
        table->set_content_cols(1);
        table->add_title_node(std::make_shared<model::DocraftText>("${title}"));
        table->add_content_node(std::make_shared<model::DocraftText>("${value}"));
        document.add_node(table);

        document.template_document();

        EXPECT_EQ(table->title_nodes()[0]->text(), "Docraft");
        auto rows = table->content_nodes();
        auto cell_text = std::dynamic_pointer_cast<model::DocraftText>(rows[0][0]);
        EXPECT_EQ(cell_text->text(), "42");
    }

    TEST_F(DocraftTemplateEngineTest, TemplateAppliesToListItems) {
        docraft::DocraftDocument document("Test Document");
        auto template_engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
        template_engine->add_template_variable("item", "Alpha");
        document.set_document_template_engine(template_engine);

        auto list = std::make_shared<model::DocraftList>();
        list->add_child(std::make_shared<model::DocraftText>("${item}"));
        list->add_child(std::make_shared<model::DocraftText>("Item ${item}"));
        document.add_node(list);

        document.template_document();
        list->update_items();

        auto first = std::dynamic_pointer_cast<model::DocraftText>(list->children()[0]);
        auto second = std::dynamic_pointer_cast<model::DocraftText>(list->children()[1]);
        EXPECT_EQ(first->text(), "Alpha");
        EXPECT_EQ(second->text(), "Item Alpha");
    }
    TEST_F(DocraftTemplateEngineTest, CreateADocumentWithTemplateEngine) {
        docraft::DocraftDocument document("Test Document");
        auto template_engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
        template_engine->add_template_variable("title", "Docraft");
        document.set_document_template_engine(template_engine);
        EXPECT_EQ(document.document_template_engine()->get_template_variable("title"), "Docraft");
        //text
        std::shared_ptr<model::DocraftText> text_node1 = std::make_shared<model::DocraftText>("${title} is a great library!");
        document.add_node(text_node1);
        auto text_node2 = std::make_shared<model::DocraftText>("${title}");
        document.add_node(text_node2);
        auto text_node3 = std::make_shared<model::DocraftText>("${description");
        document.add_node(text_node3);
        document.template_document();
        EXPECT_EQ(text_node1->text(), "Docraft is a great library!");
        EXPECT_EQ(text_node2->text(), "Docraft");
        EXPECT_EQ(text_node3->text(), "${description");
    }
}

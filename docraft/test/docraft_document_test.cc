#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "docraft_document.h"
#include "model/docraft_body.h"
#include "model/docraft_list.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_text.h"

namespace docraft::test {
    namespace {
        bool file_contains_text(const std::filesystem::path &file_path, const std::string &needle) {
            std::ifstream stream(file_path, std::ios::binary);
            if (!stream.is_open()) {
                return false;
            }
            const std::string content((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
            return content.find(needle) != std::string::npos;
        }

        std::string encode_pdf_utf16_hex_ascii(const std::string &value) {
            std::ostringstream stream;
            stream << "<FEFF";
            stream << std::uppercase << std::hex << std::setfill('0');
            for (const unsigned char ch: value) {
                stream << std::setw(4) << static_cast<int>(ch);
            }
            stream << ">";
            return stream.str();
        }
    } // namespace

    TEST(DocraftDocumentTest, GetByNameFindsRootAndChildNodes) {
        DocraftDocument document("Test Document");

        auto rect = std::make_shared<model::DocraftRectangle>();
        rect->set_name("target");

        auto list = std::make_shared<model::DocraftList>();
        list->set_name("container");

        auto child1 = std::make_shared<model::DocraftText>("Hello");
        child1->set_name("target");

        auto child2 = std::make_shared<model::DocraftText>("World");
        child2->set_name("other");

        list->add_child(child1);
        list->add_child(child2);

        document.add_node(rect);
        document.add_node(list);

        const auto matches = document.get_by_name("target");

        ASSERT_EQ(matches.size(), 2U);
        EXPECT_EQ(matches[0], rect);
        EXPECT_EQ(matches[1], child1);
    }

    TEST(DocraftDocumentTest, GetByNameReturnsEmptyWhenMissing) {
        DocraftDocument document("Test Document");

        auto rect = std::make_shared<model::DocraftRectangle>();
        rect->set_name("root");
        document.add_node(rect);

        const auto matches = document.get_by_name("missing");

        EXPECT_TRUE(matches.empty());
    }

    TEST(DocraftDocumentTest, TraverseDomVisitsNodesInPrePostOrder) {
        DocraftDocument document("Test Document");

        auto rect = std::make_shared<model::DocraftRectangle>();
        rect->set_name("root");

        auto list = std::make_shared<model::DocraftList>();
        list->set_name("container");

        auto child = std::make_shared<model::DocraftText>("Hello");
        child->set_name("child");
        list->add_child(child);

        document.add_node(rect);
        document.add_node(list);

        std::vector<std::string> visits;
        document.traverse_dom([&](const std::shared_ptr<model::DocraftNode> &node,
                                  DocraftDomTraverseOp op) {
            const auto tag = op == DocraftDomTraverseOp::kEnter ? "enter:" : "exit:";
            visits.emplace_back(tag + node->node_name());
        });

        const std::vector<std::string> expected = {
            "enter:root",
            "exit:root",
            "enter:container",
            "enter:child",
            "exit:child",
            "exit:container"
        };
        EXPECT_EQ(visits, expected);
    }

    TEST(DocraftDocumentTest, GetFirstAndLastByNameReturnExpectedNodes) {
        DocraftDocument document("Test Document");

        auto first = std::make_shared<model::DocraftRectangle>();
        first->set_name("target");

        auto list = std::make_shared<model::DocraftList>();
        list->set_name("container");

        auto middle = std::make_shared<model::DocraftText>("Middle");
        middle->set_name("target");
        list->add_child(middle);

        auto last = std::make_shared<model::DocraftText>("Last");
        last->set_name("target");

        document.add_node(first);
        document.add_node(list);
        document.add_node(last);

        EXPECT_EQ(document.get_first_by_name("target"), first);
        EXPECT_EQ(document.get_last_by_name("target"), last);
    }

    TEST(DocraftDocumentTest, GetByTypeFindsMatchingNodes) {
        DocraftDocument document("Test Document");

        auto rect1 = std::make_shared<model::DocraftRectangle>();
        auto rect2 = std::make_shared<model::DocraftRectangle>();
        auto list = std::make_shared<model::DocraftList>();
        auto child_text = std::make_shared<model::DocraftText>("Child");
        list->add_child(child_text);

        document.add_node(rect1);
        document.add_node(list);
        document.add_node(rect2);

        const auto rectangles = document.get_by_type<model::DocraftRectangle>();
        const auto texts = document.get_by_type<model::DocraftText>();

        ASSERT_EQ(rectangles.size(), 2U);
        EXPECT_EQ(rectangles[0], rect1);
        EXPECT_EQ(rectangles[1], rect2);

        ASSERT_EQ(texts.size(), 1U);
        EXPECT_EQ(texts[0], child_text);
    }

    TEST(DocraftDocumentTest, SettingDocumentTitleUpdatesMetadataTitle) {
        DocraftDocument document("Initial Title");

        document.set_document_title("Updated Title");

        ASSERT_TRUE(document.document_metadata().title().has_value());
        EXPECT_EQ(document.document_metadata().title().value(), "Updated Title");
    }

    TEST(DocraftDocumentTest, SettingMetadataWithTitleUpdatesDocumentTitle) {
        DocraftDocument document("Original Title");
        DocraftDocumentMetadata metadata;
        metadata.set_title("Metadata Title");
        metadata.set_author("Metadata Author");

        document.set_document_metadata(metadata);

        EXPECT_EQ(document.document_title(), "Metadata Title");
        ASSERT_TRUE(document.document_metadata().author().has_value());
        EXPECT_EQ(document.document_metadata().author().value(), "Metadata Author");
    }

    TEST(DocraftDocumentTest, AutoKeywordsCanBeEnabledFromCpp) {
        DocraftDocument document("Auto Keywords");
        document.add_node(std::make_shared<model::DocraftText>(
            "il parser parser metadata metadata engine engine engine and parser documento"));

        DocraftDocumentMetadata metadata;
        metadata.set_keywords("manuale");
        document.set_document_metadata(metadata);

        document.set_auto_keywords_config({
            .max_keywords = 3,
            .min_length = 4,
            .stop_word_languages = {"it", "en"}
        });
        document.enable_auto_keywords();
        document.refresh_auto_keywords();

        EXPECT_TRUE(document.auto_keywords_enabled());
        ASSERT_TRUE(document.document_metadata().keywords().has_value());
        EXPECT_EQ(document.document_metadata().keywords().value(), "manuale, engine, parser, metadata");
    }

    TEST(DocraftDocumentTest, RenderWritesMetadataSetFromCode) {
        const std::string base_title = "docraft_metadata_from_code_test";
        std::error_code ec;
        std::filesystem::remove(base_title + ".pdf", ec);
        std::filesystem::remove("Docraft Title From Code.pdf", ec);

        DocraftDocument document(base_title);
        auto body = std::make_shared<model::DocraftBody>();
        body->add_child(std::make_shared<model::DocraftText>("Simple text"));
        document.add_node(body);

        DocraftDocumentMetadata metadata;
        metadata.set_title("Docraft Title From Code");
        metadata.set_author("Docraft Author From Code");
        metadata.set_subject("Docraft Subject From Code");
        metadata.set_keywords("alpha,beta");
        document.set_document_metadata(metadata);

        const std::filesystem::path output_path = document.document_title() + ".pdf";
        EXPECT_EQ(document.document_title(), "Docraft Title From Code");
        document.render();

        ASSERT_TRUE(std::filesystem::exists(output_path));
        EXPECT_TRUE(file_contains_text(
            output_path, "/Title " + encode_pdf_utf16_hex_ascii("Docraft Title From Code")));
        EXPECT_TRUE(file_contains_text(
            output_path, "/Author " + encode_pdf_utf16_hex_ascii("Docraft Author From Code")));
        EXPECT_TRUE(file_contains_text(
            output_path, "/Subject " + encode_pdf_utf16_hex_ascii("Docraft Subject From Code")));
        EXPECT_TRUE(file_contains_text(
            output_path, "/Keywords " + encode_pdf_utf16_hex_ascii("alpha,beta")));

        std::filesystem::remove(output_path, ec);
    }
} // namespace docraft::test

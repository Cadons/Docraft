#include <gtest/gtest.h>

#include <optional>

#include "docraft/docraft_color.h"
#include "docraft/model/docraft_foreach.h"
#include "docraft/model/docraft_image.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_text.h"

namespace docraft::test::model {
    namespace {
        void expect_same_color(const docraft::DocraftColor &left, const docraft::DocraftColor &right) {
            const auto left_rgb = left.toRGB();
            const auto right_rgb = right.toRGB();
            EXPECT_FLOAT_EQ(left_rgb.r, right_rgb.r);
            EXPECT_FLOAT_EQ(left_rgb.g, right_rgb.g);
            EXPECT_FLOAT_EQ(left_rgb.b, right_rgb.b);
            EXPECT_FLOAT_EQ(left_rgb.a, right_rgb.a);
        }

        void expect_same_optional_color(const std::optional<docraft::DocraftColor> &left,
                                        const std::optional<docraft::DocraftColor> &right) {
            EXPECT_EQ(left.has_value(), right.has_value());
            if (left && right) {
                expect_same_color(*left, *right);
            }
        }
    } // namespace

    TEST(DocraftCloneTest, RectangleClonesChildrenAndState) {
        auto rect = std::make_shared<docraft::model::DocraftRectangle>();
        rect->set_name("box");
        rect->set_position({1.0F, 2.0F});
        rect->set_width(100.0F);
        rect->set_height(50.0F);
        rect->set_padding(3.0F);
        rect->set_background_color(docraft::DocraftColor(0.1F, 0.2F, 0.3F, 0.4F));
        rect->set_border_color(docraft::DocraftColor(0.5F, 0.6F, 0.7F, 0.8F));
        rect->set_border_width(2.5F);
        rect->set_visible(false);
        rect->set_z_index(4);
        rect->set_page_owner(2);
        rect->set_position_mode(docraft::model::DocraftPositionType::kAbsolute);
        rect->set_weight(0.5F);

        auto child = std::make_shared<docraft::model::DocraftText>("Hello");
        child->set_font_size(14.0F);
        rect->add_child(child);

        auto cloned_node = rect->clone();
        auto cloned = std::dynamic_pointer_cast<docraft::model::DocraftRectangle>(cloned_node);
        ASSERT_TRUE(cloned);

        EXPECT_EQ(cloned->node_name(), rect->node_name());
        EXPECT_FLOAT_EQ(cloned->position().x, rect->position().x);
        EXPECT_FLOAT_EQ(cloned->position().y, rect->position().y);
        EXPECT_FLOAT_EQ(cloned->width(), rect->width());
        EXPECT_FLOAT_EQ(cloned->height(), rect->height());
        EXPECT_FLOAT_EQ(cloned->padding(), rect->padding());
        EXPECT_EQ(cloned->visible(), rect->visible());
        EXPECT_EQ(cloned->z_index(), rect->z_index());
        EXPECT_EQ(cloned->page_owner(), rect->page_owner());
        EXPECT_EQ(cloned->position_mode(), rect->position_mode());
        EXPECT_FLOAT_EQ(cloned->weight(), rect->weight());
        expect_same_color(cloned->background_color(), rect->background_color());
        expect_same_color(cloned->border_color(), rect->border_color());
        EXPECT_FLOAT_EQ(cloned->border_width(), rect->border_width());

        ASSERT_EQ(cloned->children().size(), 1U);
        EXPECT_NE(cloned->children()[0].get(), rect->children()[0].get());
        auto cloned_text = std::dynamic_pointer_cast<docraft::model::DocraftText>(cloned->children()[0]);
        ASSERT_TRUE(cloned_text);
        EXPECT_EQ(cloned_text->text(), child->text());
        EXPECT_FLOAT_EQ(cloned_text->font_size(), child->font_size());
    }

    TEST(DocraftCloneTest, TextClonesLines) {
        auto text = std::make_shared<docraft::model::DocraftText>("Root");
        text->set_font_name("OpenSans");
        text->set_font_size(11.0F);
        text->set_color(docraft::DocraftColor(0.1F, 0.2F, 0.3F));
        text->set_alignment(docraft::model::TextAlignment::kCenter);
        text->set_style(docraft::model::TextStyle::kBold);
        text->set_underline(true);

        auto line = std::make_shared<docraft::model::DocraftText>("Line1");
        text->add_line(line);

        auto cloned_node = text->clone();
        auto cloned = std::dynamic_pointer_cast<docraft::model::DocraftText>(cloned_node);
        ASSERT_TRUE(cloned);

        EXPECT_EQ(cloned->text(), text->text());
        EXPECT_EQ(cloned->font_name(), text->font_name());
        EXPECT_FLOAT_EQ(cloned->font_size(), text->font_size());
        expect_same_color(cloned->color(), text->color());
        EXPECT_EQ(cloned->alignment(), text->alignment());
        EXPECT_EQ(cloned->style(), text->style());
        EXPECT_EQ(cloned->underline(), text->underline());

        auto cloned_lines = cloned->lines();
        ASSERT_EQ(cloned_lines.size(), 1U);
        EXPECT_NE(cloned_lines[0].get(), line.get());
        EXPECT_EQ(cloned_lines[0]->text(), line->text());
    }

    TEST(DocraftCloneTest, TableClonesNodesAndBackgrounds) {
        auto table = std::make_shared<docraft::model::DocraftTable>();
        table->set_orientation(docraft::model::LayoutOrientation::kHorizontal);
        table->set_titles({"A", "B"});
        table->set_content_cols(2);
        table->set_column_weights({1.0F, 2.0F});
        table->set_row_weights({1.0F});
        table->set_baseline_offset(0.3F);

        table->add_title_node(std::make_shared<docraft::model::DocraftText>("T1"),
                              docraft::DocraftColor(1.0F, 0.0F, 0.0F));
        table->add_title_node(std::make_shared<docraft::model::DocraftText>("T2"), std::nullopt);

        table->add_content_node(std::make_shared<docraft::model::DocraftText>("C1"),
                                docraft::DocraftColor(0.0F, 1.0F, 0.0F));
        table->add_content_node(std::make_shared<docraft::model::DocraftText>("C2"), std::nullopt);
        table->add_row_background(docraft::DocraftColor(0.5F, 0.5F, 0.5F));
        table->set_default_cell_background(docraft::DocraftColor(0.2F, 0.2F, 0.2F));

        auto cloned_node = table->clone();
        auto cloned = std::dynamic_pointer_cast<docraft::model::DocraftTable>(cloned_node);
        ASSERT_TRUE(cloned);

        EXPECT_EQ(cloned->rows(), table->rows());
        EXPECT_EQ(cloned->cols(), table->cols());
        EXPECT_EQ(cloned->content_cols(), table->content_cols());
        EXPECT_EQ(cloned->orientation(), table->orientation());
        EXPECT_EQ(cloned->titles(), table->titles());
        EXPECT_EQ(cloned->column_weights(), table->column_weights());
        EXPECT_EQ(cloned->row_weights(), table->row_weights());
        EXPECT_FLOAT_EQ(cloned->baseline_offset(), table->baseline_offset());

        const auto &original_titles = table->title_nodes();
        const auto &cloned_titles = cloned->title_nodes();
        ASSERT_EQ(cloned_titles.size(), original_titles.size());
        EXPECT_NE(cloned_titles[0].get(), original_titles[0].get());
        EXPECT_EQ(cloned_titles[0]->text(), original_titles[0]->text());

        auto original_content = table->content_nodes();
        auto cloned_content = cloned->content_nodes();
        ASSERT_EQ(cloned_content.size(), original_content.size());
        ASSERT_EQ(cloned_content[0].size(), original_content[0].size());
        auto original_cell = std::dynamic_pointer_cast<docraft::model::DocraftText>(original_content[0][0]);
        auto cloned_cell = std::dynamic_pointer_cast<docraft::model::DocraftText>(cloned_content[0][0]);
        ASSERT_TRUE(original_cell);
        ASSERT_TRUE(cloned_cell);
        EXPECT_NE(cloned_cell.get(), original_cell.get());
        EXPECT_EQ(cloned_cell->text(), original_cell->text());

        const auto &original_title_bgs = table->title_backgrounds();
        const auto &cloned_title_bgs = cloned->title_backgrounds();
        ASSERT_EQ(cloned_title_bgs.size(), original_title_bgs.size());
        for (std::size_t i = 0; i < original_title_bgs.size(); ++i) {
            expect_same_optional_color(cloned_title_bgs[i], original_title_bgs[i]);
        }

        const auto &original_content_bgs = table->content_backgrounds();
        const auto &cloned_content_bgs = cloned->content_backgrounds();
        ASSERT_EQ(cloned_content_bgs.size(), original_content_bgs.size());
        for (std::size_t i = 0; i < original_content_bgs.size(); ++i) {
            expect_same_optional_color(cloned_content_bgs[i], original_content_bgs[i]);
        }

        const auto &original_row_bgs = table->row_backgrounds();
        const auto &cloned_row_bgs = cloned->row_backgrounds();
        ASSERT_EQ(cloned_row_bgs.size(), original_row_bgs.size());
        for (std::size_t i = 0; i < original_row_bgs.size(); ++i) {
            expect_same_optional_color(cloned_row_bgs[i], original_row_bgs[i]);
        }

        expect_same_optional_color(cloned->default_cell_background(), table->default_cell_background());
    }

    TEST(DocraftCloneTest, ImageClonePreservesRawData) {
        auto image = std::make_shared<docraft::model::DocraftImage>();
        std::vector<unsigned char> raw = {1, 2, 3, 4};
        image->set_raw_data(raw, 2, 2);

        auto cloned_node = image->clone();
        auto cloned = std::dynamic_pointer_cast<docraft::model::DocraftImage>(cloned_node);
        ASSERT_TRUE(cloned);
        EXPECT_EQ(cloned->raw_data(), image->raw_data());
        EXPECT_EQ(cloned->raw_pixel_width(), image->raw_pixel_width());
        EXPECT_EQ(cloned->raw_pixel_height(), image->raw_pixel_height());
        EXPECT_EQ(cloned->has_raw_data(), image->has_raw_data());
    }

    TEST(DocraftCloneTest, ForeachClonesRenderedChildrenAndTemplateNodes) {
        auto outer = std::make_shared<docraft::model::DocraftForeach>();
        outer->set_model("${employees}");
        outer->add_child(std::make_shared<docraft::model::DocraftText>("Rendered child"));

        auto outer_text_template = std::make_shared<docraft::model::DocraftText>("${data(\"name\")}");
        auto nested = std::make_shared<docraft::model::DocraftForeach>();
        nested->set_model("${data(\"tasks\")}");
        auto nested_text_template = std::make_shared<docraft::model::DocraftText>("${data(\"task\")}");
        nested->set_template_nodes({nested_text_template});
        outer->set_template_nodes({outer_text_template, nested});

        auto cloned_node = outer->clone();
        auto cloned = std::dynamic_pointer_cast<docraft::model::DocraftForeach>(cloned_node);
        ASSERT_TRUE(cloned);
        EXPECT_EQ(cloned->model(), outer->model());

        ASSERT_EQ(cloned->children().size(), outer->children().size());
        EXPECT_NE(cloned->children()[0].get(), outer->children()[0].get());

        ASSERT_EQ(cloned->template_nodes().size(), outer->template_nodes().size());
        EXPECT_NE(cloned->template_nodes()[0].get(), outer->template_nodes()[0].get());
        EXPECT_NE(cloned->template_nodes()[1].get(), outer->template_nodes()[1].get());

        auto cloned_nested = std::dynamic_pointer_cast<docraft::model::DocraftForeach>(cloned->template_nodes()[1]);
        ASSERT_TRUE(cloned_nested);
        EXPECT_EQ(cloned_nested->model(), "${data(\"tasks\")}");
        ASSERT_EQ(cloned_nested->template_nodes().size(), 1U);
        EXPECT_NE(cloned_nested->template_nodes()[0].get(), nested->template_nodes()[0].get());
        auto cloned_nested_text = std::dynamic_pointer_cast<docraft::model::DocraftText>(
            cloned_nested->template_nodes()[0]);
        ASSERT_TRUE(cloned_nested_text);
        EXPECT_EQ(cloned_nested_text->text(), "${data(\"task\")}");
    }
} // namespace docraft::test::model

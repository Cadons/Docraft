#include <memory>

#include <gtest/gtest.h>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/utils/docraft_loom_layout_box_test_access.h"

namespace docraft::test::loom::nodes {
	namespace {
		class TestLoomNode final : public docraft::loom::nodes::DocraftLoomNode
		{
		public:
			void accept(docraft::loom::interfaces::DocraftLoomIVisitor&) override
			{
			}
		};
	}

	TEST(DocraftLoomNodeTest, StartsWithNoChildrenAndZeroInitializedLayoutBox)
	{
		TestLoomNode node;

		EXPECT_EQ(node.children_count(), 0);
		EXPECT_EQ(node.layout_box().measured_size.width, 0.0F);
		EXPECT_EQ(node.layout_box().measured_size.height, 0.0F);
		// A fresh node hasn't been sealed by Pagination yet -- page_index_or_unpaginated()
		// is the -1 "render on every page" sentinel, not a real page number.
		EXPECT_EQ(node.layout_box().page_index_or_unpaginated(), -1);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x, 0.0F);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, 0.0F);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width, 0.0F);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.height, 0.0F);
	}

	TEST(DocraftLoomNodeTest, AddsChildAndExposesItThroughChildAccessors)
	{
		TestLoomNode node;
		auto child = std::make_shared<TestLoomNode>();

		node.add_child(child);

		EXPECT_EQ(node.children_count(), 1);
		EXPECT_EQ(node.child(0).get(), child.get());
		EXPECT_EQ(node.edit_child(0).get(), child.get());
	}

	TEST(DocraftLoomNodeTest, RemovesChildAndReturnsTheRemovedNode)
	{
		TestLoomNode node;
		auto first_child = std::make_shared<TestLoomNode>();
		auto second_child = std::make_shared<TestLoomNode>();

		node.add_child(first_child);
		node.add_child(second_child);

		auto removed_child = node.remove_child(0);

		EXPECT_EQ(removed_child.get(), first_child.get());
		EXPECT_EQ(node.children_count(), 1);
		EXPECT_EQ(node.child(0).get(), second_child.get());
	}

	TEST(DocraftLoomNodeTest, AllowsEditingChildThroughMutableAccessor)
	{
		TestLoomNode node;
		auto child = std::make_shared<TestLoomNode>();

		node.add_child(child);
		node.edit_child(0)->add_child(std::make_shared<TestLoomNode>());

		EXPECT_EQ(node.child(0)->children_count(), 1);
	}

	TEST(DocraftLoomNodeTest, RejectsNullChild)
	{
		TestLoomNode node;

		EXPECT_THROW(node.add_child(nullptr), docraft::exception::InvalidInputException);
	}

	TEST(DocraftLoomNodeTest, RejectsNegativeChildIndexWhenReading)
	{
		TestLoomNode node;

		EXPECT_THROW(node.child(-1), docraft::exception::InvalidInputException);
	}

	TEST(DocraftLoomNodeTest, RejectsOutOfRangeChildIndexWhenReading)
	{
		TestLoomNode node;

		EXPECT_THROW(node.child(0), docraft::exception::InvalidInputException);
	}

	TEST(DocraftLoomNodeTest, RejectsNegativeChildIndexWhenEditing)
	{
		TestLoomNode node;

		EXPECT_THROW(node.edit_child(-1), docraft::exception::InvalidInputException);
	}

	TEST(DocraftLoomNodeTest, RejectsOutOfRangeChildIndexWhenEditing)
	{
		TestLoomNode node;

		EXPECT_THROW(node.edit_child(0), docraft::exception::InvalidInputException);
	}

	TEST(DocraftLoomNodeTest, RejectsNegativeChildIndexWhenRemoving)
	{
		TestLoomNode node;

		EXPECT_THROW(node.remove_child(-1), docraft::exception::InvalidInputException);
	}

	TEST(DocraftLoomNodeTest, RejectsOutOfRangeChildIndexWhenRemoving)
	{
		TestLoomNode node;

		EXPECT_THROW(node.remove_child(0), docraft::exception::InvalidInputException);
	}

	TEST(DocraftLoomNodeTest, PersistsLayoutBoxEdits)
	{
		TestLoomNode node;

		node.edit_layout_box().measured_size.width = 42.0F;
		node.edit_layout_box().measured_size.height = 24.0F;
		node.edit_layout_box().set_page_index(3, docraft::test::utils::LayoutBoxTestAccess::make_page_index_proof());
		node.edit_layout_box().edit_frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x = 10.0F;
		node.edit_layout_box().edit_frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y = 12.0F;
		node.edit_layout_box().edit_frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width = 50.0F;
		node.edit_layout_box().edit_frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.height = 60.0F;

		EXPECT_EQ(node.layout_box().measured_size.width, 42.0F);
		EXPECT_EQ(node.layout_box().measured_size.height, 24.0F);
		EXPECT_EQ(node.layout_box().page_index(docraft::test::utils::LayoutBoxTestAccess::make_page_index_proof()), 3);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x, 10.0F);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, 12.0F);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width, 50.0F);
		EXPECT_EQ(node.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.height, 60.0F);
	}
} // namespace docraft::test::loom::nodes


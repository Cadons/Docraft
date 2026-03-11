#include <gtest/gtest.h>

#include <vector>

#include "docraft/model/docraft_children_container_node.h"

namespace docraft::test::model {
    namespace {
        struct DrawRecorder {
            static inline std::vector<int> order;
            static void reset() { order.clear(); }
        };

        class RecordedNode : public docraft::model::DocraftNode {
        public:
            explicit RecordedNode(int id) : id_(id) {}
            void draw(const std::shared_ptr<DocraftDocumentContext> & /*context*/) override {
                DrawRecorder::order.push_back(id_);
            }
        private:
            int id_;
        };

        class RecordedContainer : public docraft::model::DocraftChildrenContainerNode {
        public:
            void draw(const std::shared_ptr<DocraftDocumentContext> &context) override {
                draw_children(context);
            }
        };
    } // namespace

    class DocraftZIndexTest : public ::testing::Test {
    protected:
        void SetUp() override {
            DrawRecorder::reset();
        }
    };

    TEST_F(DocraftZIndexTest, DrawOrderRespectsZIndexAmongSiblings) {
        auto container = std::make_shared<RecordedContainer>();
        auto a = std::make_shared<RecordedNode>(1);
        auto b = std::make_shared<RecordedNode>(2);
        auto c = std::make_shared<RecordedNode>(3);

        a->set_z_index(0);
        b->set_z_index(-1);
        c->set_z_index(2);

        container->add_child(a);
        container->add_child(b);
        container->add_child(c);

        container->draw(nullptr);

        ASSERT_EQ(DrawRecorder::order.size(), 3U);
        EXPECT_EQ(DrawRecorder::order[0], 2);
        EXPECT_EQ(DrawRecorder::order[1], 1);
        EXPECT_EQ(DrawRecorder::order[2], 3);
    }

    TEST_F(DocraftZIndexTest, DrawOrderIsStableForEqualZIndex) {
        auto container = std::make_shared<RecordedContainer>();
        auto a = std::make_shared<RecordedNode>(1);
        auto b = std::make_shared<RecordedNode>(2);
        auto c = std::make_shared<RecordedNode>(3);

        a->set_z_index(0);
        b->set_z_index(0);
        c->set_z_index(0);

        container->add_child(a);
        container->add_child(b);
        container->add_child(c);

        container->draw(nullptr);

        ASSERT_EQ(DrawRecorder::order.size(), 3U);
        EXPECT_EQ(DrawRecorder::order[0], 1);
        EXPECT_EQ(DrawRecorder::order[1], 2);
        EXPECT_EQ(DrawRecorder::order[2], 3);
    }
}

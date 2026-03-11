#include "docraft/model/docraft_position.h"

#include <gtest/gtest.h>

#include "docraft/utils/docraft_logger.h"

namespace docraft::test::model {
    class DocraftPositionTest : public ::testing::Test {
        protected:
    };
    TEST_F(DocraftPositionTest, TrasformToString) {
      docraft::model::DocraftTransform transform;
        transform.set_position({.x=10,.y=20});
        transform.set_height(10);
        transform.set_width(20);
                LOG_INFO(transform.to_string());
    }
    TEST_F(DocraftPositionTest, TrasformBoxAnchorsComputation) {
        docraft::model::DocraftTransform transform({.x=10,.y=30},30,20);
        EXPECT_EQ(transform.anchors().top_left.x,10);
        EXPECT_EQ(transform.anchors().top_left.y,30);
        EXPECT_EQ(transform.anchors().top_right.x,40);
        EXPECT_EQ(transform.anchors().top_right.y,30);
        EXPECT_EQ(transform.anchors().bottom_left.x,10);
        EXPECT_EQ(transform.anchors().bottom_left.y,10);
        EXPECT_EQ(transform.anchors().bottom_right.x,40);
        EXPECT_EQ(transform.anchors().bottom_right.y,10);
        EXPECT_EQ(transform.anchors().top_center.x,25);
        EXPECT_EQ(transform.anchors().top_center.y,30);
        EXPECT_EQ(transform.anchors().bottom_center.x,25);
        EXPECT_EQ(transform.anchors().bottom_center.y,10);
        EXPECT_EQ(transform.anchors().left_center.x,10);
        EXPECT_EQ(transform.anchors().left_center.y,20);
        EXPECT_EQ(transform.anchors().right_center.x,40);
        EXPECT_EQ(transform.anchors().right_center.y,20);
        EXPECT_EQ(transform.center().x,25);
        EXPECT_EQ(transform.center().y,20);
    }
    TEST_F(DocraftPositionTest, TrasformBoxAnchorsContentComputation) {
        docraft::model::DocraftTransform transform({.x=10,.y=30},30,20);
        transform.set_padding(5);
        EXPECT_EQ(transform.content_anchors().top_left.x,15.0F);
        EXPECT_EQ(transform.content_anchors().top_left.y,25.0F);
        EXPECT_EQ(transform.content_anchors().top_right.x,35.0F);
        EXPECT_EQ(transform.content_anchors().top_right.y,25.0F);
        EXPECT_EQ(transform.content_anchors().bottom_left.x,15.0F);
        EXPECT_EQ(transform.content_anchors().bottom_left.y,15.0F);
        EXPECT_EQ(transform.content_anchors().bottom_right.x,35.0F);
        EXPECT_EQ(transform.content_anchors().bottom_right.y,15.0F);
        EXPECT_EQ(transform.content_anchors().top_center.x,25.0F);
        EXPECT_EQ(transform.content_anchors().top_center.y,25.0F);
        EXPECT_EQ(transform.content_anchors().bottom_center.x,25.0F);
        EXPECT_EQ(transform.content_anchors().bottom_center.y,15.0F);
        EXPECT_EQ(transform.content_anchors().left_center.x,15.0F);
        EXPECT_EQ(transform.content_anchors().left_center.y,20.0F);
        EXPECT_EQ(transform.content_anchors().right_center.x,35.0F);
        EXPECT_EQ(transform.content_anchors().right_center.y,20.0F);
    }
    TEST_F(DocraftPositionTest, TestContentView) {
        docraft::model::DocraftTransform transform;
        transform.set_position({.x=10,.y=20});
        transform.set_height(10);
        transform.set_width(20);
        transform.set_padding(5);
        //check values
        LOG_INFO(transform.to_string());


    }
}

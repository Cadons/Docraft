#include "model/docraft_position.h"

#include <gtest/gtest.h>

#include "utils/docraft_logger.h"

namespace docraft::test::model {
    class DocraftPositionTest : public ::testing::Test {
        protected:
    };
    TEST_F(DocraftPositionTest, TrasformToString) {
      docraft::model::DocraftTransform transform;
        transform.top_left_={2,54};
        transform.top_right_={40,54};
        transform.bottom_left_={2,54};
        transform.bottom_right_={40,54};
        transform.right_center_={2,54};
        transform.left_center_={2,54};
        transform.bottom_right_={2,54};
        transform.bottom_center_={2,54};


        LOG_INFO(transform.to_string());
    }
    TEST_F(DocraftPositionTest, TrasformBoxAnchorsComputation) {
        docraft::model::DocraftTransform transform({10,30},30,20);
        EXPECT_EQ(transform.top_left().x,10);
        EXPECT_EQ(transform.top_left().y,30);
        EXPECT_EQ(transform.top_right().x,40);
        EXPECT_EQ(transform.top_right().y,30);
        EXPECT_EQ(transform.bottom_left().x,10);
        EXPECT_EQ(transform.bottom_left().y,10);
        EXPECT_EQ(transform.bottom_right().x,40);
        EXPECT_EQ(transform.bottom_right().y,10);
        EXPECT_EQ(transform.top_center().x,20);
        EXPECT_EQ(transform.top_center().y,30);
        EXPECT_EQ(transform.bottom_center().x,20);
        EXPECT_EQ(transform.bottom_center().y,10);
        EXPECT_EQ(transform.left_center().x,10);
        EXPECT_EQ(transform.left_center().y,20);
        EXPECT_EQ(transform.right_center().x,40);
        EXPECT_EQ(transform.right_center().y,20);
        EXPECT_EQ(transform.center().x,20);
        EXPECT_EQ(transform.center().y,20);
    }
}

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
}

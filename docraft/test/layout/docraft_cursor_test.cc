#include <gtest/gtest.h>

#include "docraft_cursor.h"

namespace docraft::test::layout {
    class docraft_cursor_test : public ::testing::Test {
    protected:
        void SetUp() override {
        }

        void TearDown() override {
        }

        DocraftCursor &cursor() {
            return cursor_;
        }

    private:
        DocraftCursor cursor_;
    };

    TEST_F(docraft_cursor_test, PositionSetters) {
        auto cursor = this->cursor();
        cursor.set_x(10);
        EXPECT_EQ(cursor.x(), 10);
        cursor.set_y(10);
        EXPECT_EQ(cursor.y(), 10);

        EXPECT_THROW(cursor.set_x(-20), std::out_of_range);
        EXPECT_THROW(cursor.set_y(-30), std::out_of_range);

        cursor.move_to(2,30);
        EXPECT_EQ(cursor.x(), 2);
        EXPECT_EQ(cursor.y(), 30);
        EXPECT_THROW(cursor.move_to(-2,3), std::out_of_range);

        //test allows negative
        cursor.allow_negative_coordinates(true);
        cursor.set_x(-2);
        EXPECT_EQ(cursor.x(), -2);
        cursor.set_y(-2);
        EXPECT_EQ(cursor.y(), -2);
    }
}

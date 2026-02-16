#include <gtest/gtest.h>

#include "docraft_cursor.h"

namespace docraft::test::layout {
    class DocraftCursorTest : public ::testing::Test {
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

    TEST_F(DocraftCursorTest, PositionSetters) {
        auto cursor = this->cursor();
        cursor.set_x(10);
        EXPECT_EQ(cursor.x(), 10);
        cursor.set_y(10);
        EXPECT_EQ(cursor.y(), 10);

        cursor.set_x(-20);
        EXPECT_EQ(cursor.x(), -20);
        cursor.set_y(-30);
        EXPECT_EQ(cursor.y(), -30);

        cursor.move_to(2,30);
        EXPECT_EQ(cursor.x(), 2);
        EXPECT_EQ(cursor.y(), 30);
        cursor.move_to(-2,3);
        EXPECT_EQ(cursor.x(), -2);
        EXPECT_EQ(cursor.y(), 3);

        //test allows negative
        cursor.set_x(-2);
        EXPECT_EQ(cursor.x(), -2);
        cursor.set_y(-2);
        EXPECT_EQ(cursor.y(), -2);
    }
}

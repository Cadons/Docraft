#include "docraft_cursor.h"

namespace docraft {
    DocraftCursor::DocraftCursor() : x_offset_(0), y_offset_(0){
        x_ = x_offset_;
        y_ = y_offset_;
    }

    DocraftCursor::~DocraftCursor() = default;

    float DocraftCursor::x() const {
        return x_;
    }

    float DocraftCursor::y() const {
        return y_;
    }

    void DocraftCursor::move_to(float x, float y) {
        move_x(x);
        move_y(y);
    }
    void DocraftCursor::reset_x() {
        x_ = x_offset_;
    }
    void DocraftCursor::reset_y() {
        y_ = y_offset_;
    }

    void DocraftCursor::move_x(float x) {
        if (x == 0) {
            x_ = x_offset_;
            return;
        }
        x_ = x;
    }
    void DocraftCursor::move_y(float y) {
        if (y == 0) {
            y_ = y_offset_;
            return;
        }
        y_ = y;
    }

    void DocraftCursor::set_offset_x(float x) {
        x_offset_ = x;
        x_+=x_offset_;
    }
    void DocraftCursor::set_offset_y(float y) {
        y_offset_ = y;
        y_-=y_offset_;
    }
    float DocraftCursor::offset_x() const {
        return x_offset_;
    }
    float DocraftCursor::offset_y() const {
        return y_offset_;
    }
} // docraft
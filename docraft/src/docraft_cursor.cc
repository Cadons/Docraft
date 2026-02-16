#include "docraft_cursor.h"

#include <valarray>
namespace docraft {
    DocraftCursor::DocraftCursor() {
    }

    DocraftCursor::~DocraftCursor() = default;

    float DocraftCursor::x() const {
        return point_.x;
    }

    float DocraftCursor::y() const {
        return point_.y;
    }

    DocraftCursorDirection DocraftCursor::direction() const {
        if (direction_stack_.empty()) {
            return DocraftCursorDirection::kVertical;//default direction
        }
        return direction_stack_.top();
    }
    void DocraftCursor::move_to(float x, float y) {
        set_x(x);
        set_y(y);
    }

    void DocraftCursor::set_x(float x) {
        point_.x = x;
    }

    void DocraftCursor::set_y(float y) {
        point_.y = y;
    }

    void DocraftCursor::reset_x() {
        set_x(0);
    }

    void DocraftCursor::reset_y() {
        set_y(0);
    }

    void DocraftCursor::push_direction(DocraftCursorDirection direction) {
        direction_stack_.push(direction);
    }
    void DocraftCursor::pop_direction() {
        if (!direction_stack_.empty()) {
            direction_stack_.pop();
        }
    }
} // docraft

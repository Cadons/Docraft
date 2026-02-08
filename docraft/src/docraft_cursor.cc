#include "docraft_cursor.h"

#include <stdexcept>
#include <valarray>

#include "utils/docraft_logger.h"
#pragma region Messages
#define ALLOW_NEGATIVE_COORDINATES_WARNING "Negative coordinates now allowed, some layout computations may not work properly!"
#define X_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE "x must be non-negative"
#define Y_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE "y must be non-negative"
#pragma endregion
namespace docraft {
    DocraftCursor::DocraftCursor() : allow_negative_coordinates_(false) {
    }

    DocraftCursor::~DocraftCursor() = default;

    float DocraftCursor::x() const {
        return point_.x;
    }

    float DocraftCursor::y() const {
        return point_.y;
    }

    bool DocraftCursor::is_negative_coordinates_allowed() const {
        return allow_negative_coordinates_;
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
        if (x < 0 && !allow_negative_coordinates_) {
            throw std::out_of_range(X_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE);
        }
        point_.x = x;
    }

    void DocraftCursor::set_y(float y) {
        if (y < 0 && !allow_negative_coordinates_) {
            throw std::out_of_range(Y_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE);
        }
        point_.y = y;
    }

    void DocraftCursor::reset_x() {
        set_x(0);
    }

    void DocraftCursor::reset_y() {
        set_y(0);
    }

    void DocraftCursor::allow_negative_coordinates(bool allow) {
        allow_negative_coordinates_ = allow;
        if (allow_negative_coordinates_) {
            //show a warning
            LOG_WARNING(ALLOW_NEGATIVE_COORDINATES_WARNING);
        }
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

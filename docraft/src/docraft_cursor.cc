#include "docraft_cursor.h"

#include <stdexcept>

#include "utils/docraft_logger.h"
#pragma region Messages
#define ALLOW_NEGATIVE_COORDINATES_WARNING "Negative coordinates now allowed, some layout computations may not work properly!"
#define X_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE "x must be non-negative"
#define Y_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE "y must be non-negative"
#pragma endregion
namespace docraft {
    DocraftCursor::DocraftCursor() : x_(0), y_(0), allow_negative_coordinates_(false) {
    }

    DocraftCursor::~DocraftCursor() = default;

    float DocraftCursor::x() const {
        return x_;
    }

    float DocraftCursor::y() const {
        return y_;
    }

    bool DocraftCursor::is_negative_coordinates_allowed() const {
        return allow_negative_coordinates_;
    }

    void DocraftCursor::move_to(float x, float y) {
        set_x(x);
        set_y(y);
    }

    void DocraftCursor::set_x(float x) {
        if (x < 0 && !allow_negative_coordinates_) {
            throw std::out_of_range(X_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE);
        }
        x_ = x;
    }

    void DocraftCursor::set_y(float y) {
        if (y < 0 && !allow_negative_coordinates_) {
            throw std::out_of_range(Y_CANNOT_BE_NEGATIVE_EXCEPTION_MESSAGE);
        }
        y_ = y;
    }

    void DocraftCursor::reset_x() {
        x_ = 0;
    }

    void DocraftCursor::reset_y() {
        y_ = 0;
        y_ = 0;
    }

    void DocraftCursor::allow_negative_coordinates(bool allow) {
        allow_negative_coordinates_ = allow;
        if (allow_negative_coordinates_) {
            //show a warning
            utils::DocraftLogger::warning(ALLOW_NEGATIVE_COORDINATES_WARNING);
        }
    }

    //TODO: remove deprecated methods


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
        x_ += x_offset_;
    }

    void DocraftCursor::set_offset_y(float y) {
        y_offset_ = y;
        y_ -= y_offset_;
    }

    float DocraftCursor::offset_x() const {
        return x_offset_;
    }

    float DocraftCursor::offset_y() const {
        return y_offset_;
    }
} // docraft

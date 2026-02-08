#pragma once
#include <stack>

#include "model/docraft_position.h"

namespace docraft {

    enum class DocraftCursorDirection {
        kHorizontal,
        kVertical
    };
    /**
     * @class DocraftCursor
     * @brief Represents a cursor in the 2D space for handling position
     */
    class DocraftCursor {
    public:
        DocraftCursor();

        ~DocraftCursor();

        float x() const;

        float y() const;

        bool is_negative_coordinates_allowed() const;

        DocraftCursorDirection direction() const;

        /**
         * @brief Resets the x-coordinate of the cursor to 0
         */
        void reset_x();

        /**
         * @brief Resets the y-coordinate of the cursor to 0
         */
        void reset_y();

        void set_x(float x);

        void set_y(float y);

        void push_direction(DocraftCursorDirection direction);
        void pop_direction();

        /**
         * @brief Moves the cursor to the specified coordinates.
         * @param x
         * @param y
         */
        void move_to(float x, float y);

        /**
         * @brief Allows or disallows negative coordinates for the cursor.
         * @param allow
         */
        void allow_negative_coordinates(bool allow);


    private:
        model::DocraftPoint point_;
        bool allow_negative_coordinates_; ///This flag controls whether negative coordinates are allowed
        std::stack<DocraftCursorDirection> direction_stack_=std::stack<DocraftCursorDirection>();
    };
} // docraft

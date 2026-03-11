/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "docraft/docraft_lib.h"
#include <stack>

#include "docraft/model/docraft_position.h"

namespace docraft {

    /**
     * @brief Cursor movement direction hints for layout handlers.
     */
    enum class DocraftCursorDirection {
        kHorizontal,
        kVertical
    };
    /**
     * @class DocraftCursor
     * @brief Tracks the current position for flow layout.
     *
     * The cursor stores a 2D point plus a stack of layout directions,
     * enabling nested horizontal/vertical layout operations.
     */
    class DOCRAFT_LIB DocraftCursor {
    public:
        /**
         * @brief Creates a cursor at origin with horizontal direction.
         */
        DocraftCursor();

        /**
         * @brief Destructor.
         */
        ~DocraftCursor();

        /**
         * @brief Returns the current x position.
         * @return X coordinate.
         */
        float x() const;

        /**
         * @brief Returns the current y position.
         * @return Y coordinate.
         */
        float y() const;


        /**
         * @brief Returns the current direction.
         * @return Current cursor direction.
         */
        DocraftCursorDirection direction() const;

        /**
         * @brief Resets the x-coordinate of the cursor to 0
         */
        void reset_x();

        /**
         * @brief Resets the y-coordinate of the cursor to 0
         */
        void reset_y();

        /**
         * @brief Sets the x position.
         * @param x New x coordinate.
         */
        void set_x(float x);

        /**
         * @brief Sets the y position.
         * @param y New y coordinate.
         */
        void set_y(float y);

        /**
         * @brief Pushes a new movement direction onto the stack.
         * @param direction Direction to push.
         */
        void push_direction(DocraftCursorDirection direction);
        /**
         * @brief Pops the current direction, restoring the previous one.
         */
        void pop_direction();
        /**
         * @brief Clears all direction hints, restoring default vertical flow.
         */
        void reset_direction();

        /**
         * @brief Moves the cursor to the specified coordinates.
         * @param x New x coordinate.
         * @param y New y coordinate.
         */
        void move_to(float x, float y);



    private:
        model::DocraftPoint point_;
        std::stack<DocraftCursorDirection> direction_stack_=std::stack<DocraftCursorDirection>();
    };
} // docraft

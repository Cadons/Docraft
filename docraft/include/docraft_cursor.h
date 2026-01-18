#pragma once
namespace docraft {
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

        //TODO: remove deprecated
        [[deprecated]]
        void move_x(float x);

        [[deprecated]]
        void move_y(float y);

        [[deprecated]]
        void set_offset_x(float x);

        [[deprecated]]
        void set_offset_y(float y);

        [[deprecated]]
        float offset_x() const;

        [[deprecated]]
        float offset_y() const;

    private:
        float x_, y_;
        bool allow_negative_coordinates_; ///This flag controls whether negative coordinates are allowed
        //TODO: remove deprecated
        [[deprecated]]
        float x_offset_;
        [[deprecated]]
        float y_offset_;
    };
} // docraft

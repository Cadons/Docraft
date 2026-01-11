#pragma once
namespace docraft {
    /**
     * @class DocraftCursor
     * @brief Represents a cursor for handling position and offset values in 2D space.
     *
     * The DocraftCursor class provides functionality to manipulate and reset
     * cursor positions and offsets along both the x and y axes. It is used
     * primarily for tracking coordinates within a graphical or layout context.
     */
    class DocraftCursor {
        public:
        DocraftCursor();
        ~DocraftCursor();
        float x() const;
        float y() const;
        void reset_x();
        void reset_y();
        void move_to(float x, float y);
        void move_x(float x);
        void move_y(float y);
        void set_offset_x(float x);
        void set_offset_y(float y);
        float offset_x() const;
        float offset_y() const;
        private:
        float x_, y_;
        float x_offset_;
        float y_offset_;
    };
} // docraft

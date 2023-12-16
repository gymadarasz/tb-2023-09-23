#pragma once

namespace madlib::graph {

    template<typename T>
    struct Point {
        T x, y;

        Point(T x = 0, T y = 0): x(x), y(y) {}

        void set(T x, T y) {
            this->x = x;
            this->y = y;
        }

        void set(const Point<T>& other) {
            set(other.x, other.y);
        }

    };

    template<typename T>
    struct Rectangle {
        T x1, y1, x2, y2;

        Rectangle(T x1 = 0, T y1 = 0, T x2 = 0, T y2 = 0, bool _fix = true):
            x1(x1), y1(y1), x2(x2), y2(y2)
        {
            if (_fix) fix();
        }

        T getWidth() const {
            return x2 - x1;
        }

        T getHeight() const {
            return y2 - y1;
        }

        void set(T x1, T y1, T x2, T y2, bool _fix = true) {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
            if (_fix) fix();
        }

        void fix() {
            if (this->x1 > this->x2) swap(this->x1, this->x2);
            if (this->y1 > this->y2) swap(this->y1, this->y2);
        }

        bool intersect(T x1 = 0, T y1 = 0, T x2 = 0, T y2 = 0) {
            // Calculate the intersection of the current viewport with the given viewport
            if (this->x1 < x1) this->x1 = x1;
            if (this->y1 < y1) this->y1 = y1;
            if (this->x2 > x2) this->x2 = x2;
            if (this->y2 > y2) this->y2 = y2;

            // Check if the resulting viewport is still valid (non-empty)
            if (this->x1 > this->x2 || this->y1 > this->y2) {
                // The viewports do not intersect, so set them to an empty viewport
                this->x1 = this->y1 = this->x2 = this->y2 = 0;
                return false;
            }

            return true;
        }

        bool intersect(const Rectangle& other) {
            return intersect(other.x1, other.y1, other.x2, other.y2);
        }

        bool insideOf(T x1, T y1, T x2, T y2) const {
            return (this->x1 >= x1 && this->y1 >= y1 && this->x2 <= x2 && this->y2 <= y2);
        }

        bool insideOf(const Rectangle& other) const {
            return insideOf(other.x1, other.y1, other.x2, other.y2);
        }

        bool containsCompletely(T x1, T y1, T x2, T y2) const {
            // Check if 'other' rectangle is completely inside 'this' rectangle
            return (x1 >= this->x1 && y1 >= this->y1 && x2 <= this->x2 && y2 <= this->y2);
        }

        bool containsCompletely(const Rectangle& other) const {
            // Check if 'other' rectangle is completely inside 'this' rectangle
            return containsCompletely(other.x1, other.y1, other.x2, other.y2);
        }

        bool containsPartially(T x1, T y1, T x2, T y2) const {
            // Check if 'other' rectangle partially overlaps with 'this' rectangle
            return !(x2 < this->x1 || x1 > this->x2 || y2 < this->y1 || y1 > this->y2);
        }

        bool containsPartially(const Rectangle& other) const {
            // Check if 'other' rectangle partially overlaps with 'this' rectangle
            return containsPartially(other.x1, other.y1, other.x2, other.y2);
        }

        bool contains(T x, T y) {
            return containsPartially(x, y, x, y);
        }

        bool contains(Point<T> point) {
            T x = point.x;
            T y = point.y;
            return contains(x, y);
        }
    };

    
    template<typename T>
    class Range {
    public:
        T begin, end;

        Range(T begin, T end): begin(begin), end(end) {}

        bool apply(const Range<T>& other) {
            bool changed = false;
            if (&other != this) {
                changed = begin != other.begin || end != other.end;
                begin = other.begin;
                end = other.end;
            }
            return changed;
        }

        bool expand(const Range<T>& other) {
            bool changed = false;
            if (&other != this) {
                T _begin = begin, _end = end;
                begin = min(other.begin, begin);
                end = max(other.end, end);
                changed = begin != _begin || end != _end;
            }
            return changed;
        }

        bool limit(const Range<T>& other) {
            bool changed = false;
            if (&other != this) {
                T _begin = begin, _end = end;
                begin = max(other.begin, begin);
                end = min(other.end, end);
                changed = begin != _begin || end != _end;
            }
            return changed;
        }

    };

    template<typename T>
    class MinMax {
    public:
        T min, max;
        MinMax(T min, T max): min(min), max(max) {}
    };

    class TimeRange: public Range<ms_t> {
    public:
        TimeRange(
            ms_t begin, 
            ms_t end
        ):
            Range(begin, end)
        {}
    };

}
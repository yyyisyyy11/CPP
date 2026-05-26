#ifndef GOLDEN_RECTANGLE_H
#define GOLDEN_RECTANGLE_H

#include <cassert>
#include <cmath>

template <typename T>
class GoldenRectangle {
private:
    T length;
    T width;

public:
    GoldenRectangle(T l, T w) : length(l), width(w) {}

    bool isGolden() const {
        T longSide = (length > width) ? length : width;
        T shortSide = (length > width) ? width : length;

        assert(shortSide != 0 && "Short side cannot be 0 (division by zero)");

        double ratio = static_cast<double>(longSide) / static_cast<double>(shortSide);
        // Compare with tolerance for floating point
        return std::abs(ratio - 1.618) <= 1e-3;
    }
};

#endif

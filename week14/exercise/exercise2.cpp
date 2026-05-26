#include "golden_rectangle.h"
#include <iostream>

int main() {
    GoldenRectangle<int> rect0(1618, 10);
    std::cout << "1618x10 is golden Rectangle: " << std::boolalpha << rect0.isGolden() << std::endl;

    GoldenRectangle<int> rect1(34, 21);
    std::cout << "34x21 is golden Rectangle: " << std::boolalpha << rect1.isGolden() << std::endl;

    GoldenRectangle<double> rect2(1.0, 1.618);
    std::cout << "1.0x1.618 is golden Rectangle: " << rect2.isGolden() << std::endl;

    GoldenRectangle<float> rect3(10.0f, 6.0f);
    std::cout << "10.0x6.0 is golden Rectangle: " << rect3.isGolden() << std::endl;

    GoldenRectangle<int> rect4(10, 0);
    std::cout << "10x0 is golden Rectangle: " << rect4.isGolden() << std::endl;

    return 0;
}

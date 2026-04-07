#include <iostream>
#include "swap.h"

int main()
{
    int a = 10, b = 20;

    std::cout << "Before swap: a = " << a << ", b = " << b << std::endl;
    swap(a, b);
    std::cout << "After swap:  a = " << a << ", b = " << b << std::endl;

    return 0;
}

#include <iostream>
#include "rational.h"
using namespace std;

int main() {
    Rational a = 10;
    Rational b(1, 2);
    Rational c = a * b;
    cout << "c = " << c << endl;
    Rational d = 2 * a;
    cout << "d = " << d << endl;
    Rational e = b * 3;
    cout << "e = " << e << endl;
    Rational f = 2 * 3;
    cout << "f = " << f << endl;
    return 0;
}

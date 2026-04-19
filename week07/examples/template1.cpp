#include <iostream>
#include <typeinfo>
using namespace std;

template<typename T>
T sum(T x, T y)
{
    cout << "The input type is " << typeid(T).name() << endl;
    return x +
     y;
}
// Explicitly instantiate
template double sum<double>(double, double);

int main()
{
    auto val1 = sum(4.1, 5.2);
    cout << val1 << endl;
    auto val2 = sum((short)4, (short)5);
    cout << val2 << endl;
    return 0;
}

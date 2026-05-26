#include <iostream>
#include <string>
using namespace std;

template <class T1, class T2>
class Pair
{
public:
    T1 key;
    T2 value;

    Pair(T1 k, T2 v) : key(k), value(v) {}

    bool operator<(const Pair<T1, T2> &p) const;

    template <class U1, class U2>
    friend ostream &operator<<(ostream &out, const Pair<U1, U2> &p);
};

template <class T1, class T2>
bool Pair<T1, T2>::operator<(const Pair<T1, T2> &p) const
{
    return this -> key < p.key;
}

template <class U1, class U2>
ostream &operator<<(ostream &out, const Pair<U1, U2> &p)
{
    out << "{" << p.key << ", " << p.value << "}" << endl;
    return out;
}

int main()
{
    Pair<string, int> one("Tom", 19);
    Pair<string, int> two("Alice", 20);

    if (one < two)
        cout << one;
    else
        cout << two;

    return 0;
}

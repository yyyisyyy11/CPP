#include <iostream>
using namespace std;

class ConstMember
{
private:
    const int m_a;
public:
    ConstMember(int a) : m_a(a) {}

    void display() const
    {
        cout << "The value of the const member variable m_a is: " << m_a << endl;
    }
};

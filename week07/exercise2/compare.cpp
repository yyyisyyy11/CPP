#include <iostream>
#include <string>
using namespace std;

// Struct definition
struct stuinfo {
    string name;
    int age;
};

// Function template
template <typename T>
int Compare(const T &a, const T &b)
{
    if (a > b) return 1;
    if (a < b) return -1;
    return 0;
}

// Explicit specialization for stuinfo (compare by age only)
template <>
int Compare<stuinfo>(const stuinfo &a, const stuinfo &b)
{
    if (a.age > b.age) return 1;
    if (a.age < b.age) return -1;
    return 0;
}

int main()
{
    // Test with two integers
    cout << "Compare of the two integers:" << Compare(3, 5) << endl;

    // Test with two floats
    cout << "Compare of the two floats:" << Compare(3.5, 2.1) << endl;

    // Test with two characters
    cout << "Compare of the two characters:" << Compare('a', 'a') << endl;

    // Test with two stuinfo structs
    stuinfo s1 = {"Alice", 20};
    stuinfo s2 = {"Bob", 18};
    cout << "Compare of the two structs:" << Compare(s1, s2) << endl;

    return 0;
}

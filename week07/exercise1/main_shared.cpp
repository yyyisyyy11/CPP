#include <iostream>
#include <cstdio>
#include "vabs.h"

using namespace std;

// Function pointer types for printing addresses
typedef bool (*vabs_int_t)(int *, size_t);
typedef bool (*vabs_float_t)(float *, size_t);
typedef bool (*vabs_double_t)(double *, size_t);

int main()
{
    cout << "===== Shared Library (libvabs.so) Test =====" << endl;

    vabs_int_t    f1 = static_cast<vabs_int_t>(vabs);
    vabs_float_t  f2 = static_cast<vabs_float_t>(vabs);
    vabs_double_t f3 = static_cast<vabs_double_t>(vabs);

    printf("Address of vabs(int*,    size_t): %p\n", (void *)f1);
    printf("Address of vabs(float*,  size_t): %p\n", (void *)f2);
    printf("Address of vabs(double*, size_t): %p\n", (void *)f3);

    // Test with int array
    int arr_i[] = {-1, -2, 3, -4, 5};
    size_t n_i = sizeof(arr_i) / sizeof(arr_i[0]);
    cout << "\nInt array before vabs:   ";
    for (size_t i = 0; i < n_i; i++) cout << arr_i[i] << " ";
    vabs(arr_i, n_i);
    cout << "\nInt array after vabs:    ";
    for (size_t i = 0; i < n_i; i++) cout << arr_i[i] << " ";

    // Test with float array
    float arr_f[] = {-1.1f, 2.2f, -3.3f};
    size_t n_f = sizeof(arr_f) / sizeof(arr_f[0]);
    cout << "\n\nFloat array before vabs: ";
    for (size_t i = 0; i < n_f; i++) cout << arr_f[i] << " ";
    vabs(arr_f, n_f);
    cout << "\nFloat array after vabs:  ";
    for (size_t i = 0; i < n_f; i++) cout << arr_f[i] << " ";

    // Test with double array
    double arr_d[] = {-10.5, 20.3, -30.7, 40.1};
    size_t n_d = sizeof(arr_d) / sizeof(arr_d[0]);
    cout << "\n\nDouble array before vabs: ";
    for (size_t i = 0; i < n_d; i++) cout << arr_d[i] << " ";
    vabs(arr_d, n_d);
    cout << "\nDouble array after vabs:  ";
    for (size_t i = 0; i < n_d; i++) cout << arr_d[i] << " ";

    // Test null pointer
    cout << "\n\nvabs(nullptr, 5) returns: " << (vabs((int *)nullptr, 5) ? "true" : "false");
    cout << endl;

    return 0;
}

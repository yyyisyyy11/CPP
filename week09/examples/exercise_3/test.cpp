#include "Complex.cpp"

int main(){
    Complex c1(1, 2);
    Complex c2(3, -4);
    Complex c3;

    c1.display();
    c2.display();
    c3.display();

    c1.add(c2).display();
    c1.subtract(c2).display();

    return 0;
}

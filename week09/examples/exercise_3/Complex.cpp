#include <iostream>
/*Define a class named Complex to represent and perform arithmetic operations with complex numbers. A
complex number is expressed in the form:
realPart + imaginaryPart * i
1. Use two member variables to store the private data of the class (representing the real and imaginary parts).
2. Provide a constructor that allows an object to be initialized when it is declared. The constructor should
include default values for both parts in case no initial values are provided.
3. Implement public member functions to perform at least the following tasks:
• add: Add two Complex numbers.
• subtract: Subtract two Complex numbers.
• display: Print a Complex number in a human-readable format, such as a + bi or a - bi, where a is the real part and b is the
imaginary part.
4. Write a main function to thoroughly test all the implemented functionality of your Complex class*/
class Complex{
    private:
        int real;
        int imag;
    public:
        Complex(int r = 0, int i = 0) : real(r), imag(i) {}
        Complex add(Complex c){
            Complex result;
            result.real = c.real + this->real;
            result.imag = c.imag + this->imag;
            return result;
        }

        Complex subtract(Complex c){
            Complex result;
            result.real = this->real - c.real;
            result.imag = this->imag - c.imag;
            return result;
        }
        
        void display(){
            if(this->imag < 0){
                std::cout<< this -> real << this -> imag << "i" << std::endl;
            }
            else{
                std::cout << this->real << "+" << this->imag << "i" << std::endl;
            }
        }
};

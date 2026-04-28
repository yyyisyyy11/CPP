#pragma once
#include <iostream>
#include <string>

class Complex {
private:
    double real;
    double imag;

public:
    Complex() : real(0), imag(0) {}
    Complex(double r, double i) : real(r), imag(i) {}

    double getReal() const { return real; }
    double getImag() const { return imag; }

    // 共轭 ~
    Complex operator~() const {
        return Complex(real, -imag);
    }

    // ==
    bool operator==(const Complex &rhs) const {
        return real == rhs.real && imag == rhs.imag;
    }

    // !=
    bool operator!=(const Complex &rhs) const {
        return !(*this == rhs);
    }

    // 赋值 =
    Complex &operator=(const Complex &rhs) {
        real = rhs.real;
        imag = rhs.imag;
        return *this;
    }

    // +
    friend Complex operator+(const Complex &lhs, const Complex &rhs) {
        return Complex(lhs.real + rhs.real, lhs.imag + rhs.imag);
    }

    // - (Complex - Complex)
    friend Complex operator-(const Complex &lhs, const Complex &rhs) {
        return Complex(lhs.real - rhs.real, lhs.imag - rhs.imag);
    }

    // - (Complex - int)
    friend Complex operator-(const Complex &lhs, double rhs) {
        return Complex(lhs.real - rhs, lhs.imag);
    }

    // * (Complex * Complex)
    friend Complex operator*(const Complex &lhs, const Complex &rhs) {
        return Complex(lhs.real * rhs.real - lhs.imag * rhs.imag,
                       lhs.real * rhs.imag + lhs.imag * rhs.real);
    }

    // * (int * Complex)
    friend Complex operator*(double lhs, const Complex &rhs) {。/；/
        return Complex(lhs * rhs.real, lhs * rhs.imag);
    }

    // * (Complex * int)
    friend Complex operator*(const Complex &lhs, double rhs) {
        return Complex(lhs.real * rhs, lhs.imag * rhs);
    }

    // <<
    friend std::ostream &operator<<(std::ostream &os, const Complex &c) {
        os << c.real;
        if (c.imag >= 0)
            os << "+" << c.imag << "i";
        else
            os << c.imag << "i";
        return os;
    }

    // >>
    friend std::istream &operator>>(std::istream &is, Complex &c) {
        is >> c.real >> c.imag;
        return is;
    }
};

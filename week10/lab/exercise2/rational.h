#pragma once
#include <iostream>

class Rational {
private:
    static int id;
    int numerator;
    int denominator;

public:
    Rational(int n = 0, int d = 1) : numerator(n), denominator(d) {
        std::cout << "Construct_" << ++id << ", n:" << numerator
                  << " , d:" << denominator << std::endl;
    }

    int getN() const { return numerator; }
    int getD() const { return denominator; }

    friend std::ostream &operator<<(std::ostream &os, const Rational &rhs) {
        os << rhs.numerator << "/" << rhs.denominator;
        return os;
    }
};

int Rational::id = 0;

const Rational operator*(const Rational &lhs, const Rational &rhs) {
    return Rational(lhs.getN() * rhs.getN(), lhs.getD() * rhs.getD());
}

const Rational operator*(const Rational &lhs, int rhs) {
    return Rational(lhs.getN() * rhs, lhs.getD());
}

const Rational operator*(int lhs, const Rational &rhs) {
    return Rational(lhs * rhs.getN(), rhs.getD());
}

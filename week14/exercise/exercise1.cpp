#include <iostream>
#include <string>

class OutOfRangeException : public std::exception {
private:
    std::string message;
public:
    OutOfRangeException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept { return message.c_str(); }
    ~OutOfRangeException() noexcept {}
};

float calculateAverage(int mark1, int mark2, int mark3, int mark4) {
    int marks[] = {mark1, mark2, mark3, mark4};
    for (int i = 0; i < 4; i++) {
        if (marks[i] < 0 || marks[i] > 100) {
            throw OutOfRangeException(
                "Mark " + std::to_string(marks[i]) + " is out of range [0, 100]"
            );
        }
    }
    return (mark1 + mark2 + mark3 + mark4) / 4.0f;
}

int main() {
    try {
        float avg = calculateAverage(80, 90, 75, 85);
        std::cout << "Average: " << avg << std::endl;
    }
    catch (const OutOfRangeException& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    try {
        float avg = calculateAverage(80, 110, 75, 85);
        std::cout << "Average: " << avg << std::endl;
    }
    catch (const OutOfRangeException& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    try {
        float avg = calculateAverage(80, 90, -5, 85);
        std::cout << "Average: " << avg << std::endl;
    }
    catch (const OutOfRangeException& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

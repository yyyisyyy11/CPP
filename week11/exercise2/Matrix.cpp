#include <iostream>
#include <memory>
using namespace std;

class Matrix {
    private:
        int rows, cols;
        shared_ptr<float[]> data;

    public:
        Matrix(int r, int c){
            this->rows = r;
            this->cols = c;
            this->data = make_shared<float[]>(r*c);
            this->data[0] = 1;
        }
    
        Matrix() : rows(0), cols(0), data(nullptr) {}

        Matrix operator+(const Matrix& other) const{
            Matrix result(rows, cols);
            if(rows != other.rows || cols != other.cols){
                throw std::invalid_argument("Error");
            }
            for (int i = 0; i < rows * cols; i++) {
                result.data[i] = this->data[i] + other.data[i];
            }
            return result;
        }
    
        friend ostream& operator<<(ostream& out, const Matrix& m);
};

ostream& operator<<(ostream& out, const Matrix& m){
    
    for(int i = 0; i < m.rows; i++){
        for(int j = 0; j < m.cols; j++){
            out << m.data[i * m.cols + j] << " ";
        }
        out << endl;
    }
    return out;
}

int main(){
    Matrix a(3, 4);
    Matrix b(3, 4);22
    Matrix c = a + b;
    Matrix d = a;
    d = b;
    cout<< c << endl;
    return 0;
}

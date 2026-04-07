#include <iostream>
#include <float.h>

struct Matrix
{
    size_t rows;
    size_t cols;
    float * pData;
};

int main()
{
    using namespace std;

    Matrix matA = {3,4};
    matA.pData = new float[matA.rows * matA.cols]{1.f, 2.f, 3.f};

    Matrix matB = {4,8};
    matB.pData = new float[matB.rows * matB.cols]{10.f, 20.f, 30.f};

    Matrix matC = {4, 2};
    matC.pData = new float[matC.rows * matC.cols]{100.f, 200.f, 300.f};

    // some operations on the matrices

    // 输入检查：确保矩阵合法
    if (matA.rows == 0 || matA.cols == 0 || matA.pData == nullptr)
    {
        cerr << "matA is invalid!" << endl;
        return -1;
    }
    if (matB.rows == 0 || matB.cols == 0 || matB.pData == nullptr)
    {
        cerr << "matB is invalid!" << endl;
        return -1;
    }
    if (matC.rows == 0 || matC.cols == 0 || matC.pData == nullptr)
    {
        cerr << "matC is invalid!" << endl;
        return -1;
    }

    float maxa = -FLT_MAX;
    float maxb = -FLT_MAX;
    float maxc = -FLT_MAX;

    //find max value of matA (单层循环)
    size_t totalA = matA.rows * matA.cols;
    for (size_t i = 0; i < totalA; i++)
    {
        if (matA.pData[i] > maxa)
            maxa = matA.pData[i];
    }

    //find max value of matB (单层循环)
    size_t totalB = matB.rows * matB.cols;
    for (size_t i = 0; i < totalB; i++)
    {
        if (matB.pData[i] > maxb)
            maxb = matB.pData[i];
    }

    //find max value of matC (单层循环)
    size_t totalC = matC.rows * matC.cols;
    for (size_t i = 0; i < totalC; i++)
    {
        if (matC.pData[i] > maxc)
            maxc = matC.pData[i];
    }

    cout << "max(matA) = " << maxa << endl;
    cout << "max(matB) = " << maxb << endl;
    cout << "max(matC) = " << maxc << endl;


    delete [] matA.pData;
    delete [] matB.pData;
    delete [] matC.pData;

    return 0;
}
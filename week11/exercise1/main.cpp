#include <iostream>
#include <memory>
using namespace std;

int main()
{
    shared_ptr<double> pd = make_shared<double>(5);
    cout << "*pd = " << *pd << endl;
    
    shared_ptr<double> pshared = pd;
    cout << "*pshred = " << *pshared << endl;
    
    shared_ptr<string> pstr = make_shared<string>("Hello World!");
    cout << "*pstr = " << *pstr << endl;
    return 0;
}
#include<iostream>
using namespace std;
class Demo
{
    private:
        int id;
    public:
        void display(){
            cout<<"this is: "<<this<<", id is:"<<this->id<<endl;
        }
    Demo() { this->id = 0; }
    Demo(int cid){
        this->id=cid;
    }
    static int num;
    static void static_display()
    {
        cout<<"The value of the static num is: "<<num<<endl;
    }
};

// 必须在类外单独初始化静态变量
int Demo::num = 100; 

int main()
{
    Demo obj;
    Demo obj1(1);

    obj.display();
    obj1.display();

    Demo::static_display();

    return 0;
}

// 包含了 ConstMember 的定义以便能够编译通过
#include "ConstMember.cpp"

int main()
{
    ConstMember o1{666};
    ConstMember o2{42};

    o1.display();
    o2.display();

    o1 = o2;

    return 0;
}

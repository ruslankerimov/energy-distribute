#include <iostream>

using namespace std;

class A
{
public:
    void parse()
    {
        show();
    }

    virtual void show()
    {
        cout << "opaA";
    }
};

class B: public A
{
public:
    void show()
    {
        cout << "opaB";
    }
};

int main()
{
    B b;

    b.parse();

    return 0;
}

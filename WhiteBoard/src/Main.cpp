#include <iostream>
#include <vector>
#include <memory>
#include <DirectXMath.h>

class A
{
public:
	A() {
		std::cout << "abc";
	}
	A(int a)
		: A()
	{
		std::cout << "cc";
	}

	int k;

};

int main()
{
	A a(3);

}
#include <iostream>
#include <vector>
#include <memory>
#include <DirectXMath.h>

class Some 
{
	virtual void some(){

	}
};

class SS : public Some
{
public:
	virtual void some() override
	{
		std::cout << "hello";
	}

};

struct A
{
	int x;

	A& operator-()
	{
		x = -x;
		return *this;
	}
};


int main()
{
	auto v1 = DirectX::XMMatrixOrthographicLH(5000.0f, 5000.0f, -5000.0f, 5000.0f);
	auto v2 = DirectX::XMMatrixOrthographicLH(1, 1, -5000.0f, 5000.0f);
	auto v3 = DirectX::XMMatrixOrthographicLH(5000.0f, 5000.0f, -1, 1);
	auto v4 = DirectX::XMMatrixOrthographicLH(5000.0f, 5000.0f, -5000.0f, 5000.0f);

}
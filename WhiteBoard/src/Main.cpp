#include <iostream>

#include <string>
#include <filesystem>
#include <DirectXMath.h>

void some(uint32_t a, uint32_t b)
{
	std::cout << "some1" << std::endl;
}

void some(uint32_t a, bool b)
{
	std::cout << "some2" << std::endl;
}

int main()
{
	DirectX::XMVECTOR vec;

	vec = { 0.0f, 0.0f, 0.0f, 0.0f };
}
#include <iostream>

#include <DirectXMath.h>
#include <vector>
#include <chrono>

int main()
{
	std::chrono::system_clock::time_point t = std::chrono::system_clock::now();

	const int size = 100'000'000;
	int* arr = new int[size];
	for (int i = 0; i < size; ++i)
		arr[i] = i;

	std::chrono::duration<float> dur = std::chrono::system_clock::now() - t;
	std::cout << dur.count() << std::endl;

}
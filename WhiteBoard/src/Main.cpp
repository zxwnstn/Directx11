#include <iostream>

template<typename T, typename elem>
void some(elem v)
{
	std::cout << v << std::endl;

}

int main()
{
	int* ptr;
	some<int>(ptr);
}
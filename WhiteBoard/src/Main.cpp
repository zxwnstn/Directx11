#include <iostream>
#include <vector>
#include <memory>

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


int main()
{
	std::vector<std::shared_ptr<Some>> s;

	std::shared_ptr<Some> ptr;
	ptr.reset(new SS);

	s.push_back(ptr);

}
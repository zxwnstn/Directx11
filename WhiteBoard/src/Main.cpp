#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <string>
#include <sstream>

std::unordered_map<std::string, std::vector<float>> v;

int main()
{
	std::vector<float> f1;
	f1.push_back(3);
	std::vector<float> f2;
	f2.push_back(1);
	std::vector<float> f3;
	f3.push_back(2);

	v["asdf"] = f1;
	v["ss"] = f2;
	v["eres"] = f3;

	std::stringstream ss;
	boost::archive::text_oarchive oa(ss);
	oa << v;
	
	std::unordered_map<std::string, std::vector<float>> v2;
	boost::archive::text_iarchive ia(ss);
	ia >> v2;
}
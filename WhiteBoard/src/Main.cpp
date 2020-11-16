#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <string>
#include <sstream>
#include <fstream>

struct some
{
	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & v;
	}

	std::vector<std::string> v;
};

int main()
{
	some s;
	s.v.push_back("abc");
	s.v.push_back("abc");
	s.v.push_back("abc");

	std::ofstream ofs("filename");
	boost::archive::text_oarchive oa(ofs);
	// write class instance to archive
	oa << s;
	
}
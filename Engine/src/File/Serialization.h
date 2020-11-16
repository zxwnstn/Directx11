#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

namespace Engine {

	class Serializer
	{
	public:
		template<typename T>
		static void Write(const std::string& filename, const T& target)
		{
			std::ofstream ofs(filename);
			boost::archive::text_oarchive oa(ofs);
			oa << target;
			ofs.close();
		}

		template<typename T>
		static void Read(const std::string& filename, T& target)
		{
			std::ifstream ifs(filename);
			boost::archive::text_iarchive ia(ifs);
			ia >> target;
			ifs.close();
		}
	};

}

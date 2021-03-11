

#pragma once

#include "Common/Mesh.h"

namespace Engine {

	struct Joint
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Parent;
			ar & Name;
			ar & Offset.m[0];
			ar & Offset.m[1];
			ar & Offset.m[2];
			ar & Offset.m[3];
		}

		Joint();

		int Parent;
		std::string Name;
		mat4 Offset;
	};

	struct Skeleton
	{
		std::string Name;

		std::vector<Joint> Joints;
		std::unordered_map<std::string, std::vector<ControlPoint>> ControlPoints;
	};

	class SkeletonArchive
	{
	public:
		static bool Add(const std::string& name);
		static std::shared_ptr<Skeleton> Get(const std::string& name);
		static bool Has(const std::string& name);
		static void Shutdown();
		static std::vector<std::string> GetAllName();
	};

}

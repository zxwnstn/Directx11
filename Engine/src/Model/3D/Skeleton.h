#pragma once

#include "File/SerializedData.h"

namespace Engine {

	struct Joint
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Parent;
			ar & Name;
			ar & Offset.r[0].m128_f32;
			ar & Offset.r[1].m128_f32;
			ar & Offset.r[2].m128_f32;
			ar & Offset.r[3].m128_f32;
		}

		int Parent;
		std::string Name;
		DirectX::XMMATRIX Offset;
	};

	struct Skeleton
	{
		std::string Name;

		std::vector<Joint> Joints;

		std::vector<ControlPoint> ControlPoints;
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	class SkeletonArchive
	{
	public:
		static bool Add(const std::string& name);
		static std::shared_ptr<Skeleton> Get(const std::string& name);
		static bool Has(const std::string& name);
		static void Shutdown();
	};

}

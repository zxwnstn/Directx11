#pragma once

namespace Engine {

	struct Vertex
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Position;
			ar & UV;

			ar & Normal;
			ar & BiNormal;
			ar & Tangent;

			ar & BoneWeight;
			ar & BoneWeightr;
			ar & BoneIndex;
			ar & BoneIndexr;

			ar & MaterialIndex;
		}
		vec3 Position;
		vec2 UV;

		vec3 Normal;
		vec3 BiNormal;
		vec3 Tangent;

		vec4 BoneWeight;
		vec4 BoneWeightr;

		uvec4 BoneIndex;
		uvec4 BoneIndexr;

		int MaterialIndex = 0;

		bool check();
	};

	struct ControlPoint
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Position;

			ar & BoneWeight;
			ar & BoneWeightr;

			ar & BoneIndex;
			ar & BoneIndexr;
		}
		vec3 Position;

		vec4 BoneWeight;
		vec4 BoneWeightr;

		uvec4 BoneIndex;
		uvec4 BoneIndexr;

		int MaterialIndex = 0;
		uint8_t i = 0;

		void push(float weight, uint32_t index);
	};

}

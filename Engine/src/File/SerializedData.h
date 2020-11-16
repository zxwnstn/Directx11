#pragma once

namespace Engine {

	struct vec2
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m[0];
			ar & m[1];
		}
		float m[2]{ 0, };
	};
	struct vec3
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m[0];
			ar & m[1];
			ar & m[2];
		}
		float m[3]{ 0, };
	};
	struct vec4
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m[0];
			ar & m[1];
			ar & m[2];
			ar & m[3];
		}
		float m[4]{ 0, };
	};
	struct mat4
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ar & m[i][j];
				}
			}
		}
		float m[4][4]{ 0, };
	};

	struct uvec2
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m[0];
			ar & m[1];
		}
		unsigned int m[2]{ 0, };
	};
	struct uvec3
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m[0];
			ar & m[1];
			ar & m[2];
		}
		unsigned int m[3]{ 0, };
	};
	struct uvec4
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m[0];
			ar & m[1];
			ar & m[2];
			ar & m[3];
		}
		unsigned int m[4]{ 0, };
	};
	struct umat4
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ar & m[i][j];
				}
			}
		}
		unsigned int m[4][4]{ 0, };
	};

	struct Vertex
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Position;
			ar & UV;
			ar & TintColor;

			ar & Normal;
			ar & BiNormal;
			ar & Tangent;
			ar & BoneWeight;
			ar & BoneIndex;
		}
		vec3 Position;
		vec2 UV;
		vec4 TintColor{1.0, 1.0f, 1.0f, 1.0f};

		vec3 Normal;
		vec3 BiNormal;
		vec3 Tangent;

		vec4 BoneWeight;
		uvec4 BoneIndex;
	};

	struct ControlPoint
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Position;
			ar & BoneWeight;
			ar & BoneIndex;
		}
		vec3 Position;
		vec4 BoneWeight;
		uvec4 BoneIndex;

		uint8_t i = 0;
		inline void push(float weight, uint32_t index)
		{
			BoneWeight.m[i] = weight;
			BoneIndex.m[i] = index;
			++i;
		}
	};

}

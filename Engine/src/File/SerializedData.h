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

			ar & Normal;
			ar & BiNormal;
			ar & Tangent;

			ar & BoneWeight;
			ar & BoneWeightr;
			ar & BoneIndex;
			ar & BoneIndexr;
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

		bool check()
		{
			for (int i = 0; i < 3; ++i)
			{
				if (!(-1000.0f <= Position.m[i]  && Position.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
				if (!(-1000.0f <= Normal.m[i] && Normal.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
				if (!(-1000.0f <= BiNormal.m[i] && BiNormal.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
				if (!(-1000.0f <= Tangent.m[i] && Tangent.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
			}

			for (int i = 0; i < 4; ++i)
			{
				if (!(-1000.0f <= BoneWeight.m[i] && BoneWeight.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
				if (!(-1000.0f <= BoneIndex.m[i] && BoneIndex.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
				if (!(-1000.0f <= BoneWeightr.m[i] && BoneWeightr.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
				if (!(-1000.0f <= BoneIndexr.m[i] && BoneIndexr.m[i] <= 1000.0f))
				{
					__debugbreak();
				}
			}
			return true;
		}
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

		uint8_t i = 0;
		inline void push(float weight, uint32_t index)
		{
			if (i >= 4)
			{
				std::cout << "Bone Weight have over 4\n";
				BoneWeightr.m[i % 4] = weight;
				BoneIndexr.m[i % 4] = index;
				return;
			}
			BoneWeight.m[i] = weight;
			BoneIndex.m[i] = index;
			++i;
		}
	};

}

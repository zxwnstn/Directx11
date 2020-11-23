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
		union {
			float m[2]{ 0, };
			struct {
				float x, y;
			};
		};

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
		union {
			float m[3]{ 0, };
			struct {
				float x, y, z;
			};
		};
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
		union {
			float m[4]{ 0, };
			struct {
				float x, y, z, w;
			};
		};
		vec4() = default;
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
		union {
			float m[4][4]{ 0, };
			struct {
				vec4 _1, _2, _3, _4;
			};
		};
		mat4() {}
	};

	struct uvec2
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & m[0];
			ar & m[1];
		}
		union {
			unsigned int m[2]{ 0, };
			struct {
				uint32_t x, y;
			};
		};
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
		union {
			unsigned int m[3]{ 0, };
			struct {
				uint32_t x, y, z;
			};
		};
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
		union {
			unsigned int m[4]{ 0, };
			struct {
				uint32_t x, y, z, w;
			};
		};
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
		union {
			unsigned int m[2]{ 0, };
			struct {
				uint32_t x, y;
			};
		};
	};


}

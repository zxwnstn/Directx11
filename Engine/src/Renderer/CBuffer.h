#pragma once

#include "Common/Camera.h"
#include "Common/Light.h"
#include "Common/Material.h"

namespace Engine::CBuffer {

	enum class Type
	{
		Camera, Transform, Bone, 
		Environment, Light, Material,
		Materials, None
	};

	struct Camera
	{
		mat4 View;
		mat4 Projection;
		vec3 Position;
		int padding;

		void Upload(Engine::Camera& other);
	};

	struct Transform
	{
		mat4 Translate;
		mat4 Rotate;
		mat4 Scale;

		void Upload(Engine::Transform& other);
	};

	struct Light
	{
		vec4 Position;
		vec4 Direction;
		vec4 Color;
		float Intensity;
		int Type;
		int padding[2];

		void Upload(const Engine::Light& other);
	};

	struct Bone
	{
		mat4 SkinnedTransform[100];
		void Upload(mat4* skinnedTransform);
	};

	struct Environment
	{
		mat4 WorldMatrix;
		vec3 Ambient;
		int padding;

		void Upload(const Engine::Environment& other);
	};

	struct Material
	{
		vec4 Ambient;
		vec4 Diffuse;
		vec4 Specular;
		vec4 Emissive;
		vec4 Fresnel;
		float Shiness;
		int MapMode;

		void Upload(const Engine::Material& other);
	};

	struct Materials
	{
		vec4 Ambient[MAXIMUM_PART];
		vec4 Diffuse[MAXIMUM_PART];
		vec4 Specular[MAXIMUM_PART];
		vec4 Emissive[MAXIMUM_PART];
		vec4 Fresnel[MAXIMUM_PART];
		
		float Shiness[MAXIMUM_PART];
		unsigned int MMode[MAXIMUM_PART]{0, };

		void Upload(const Engine::MaterialSet& other);
	};

	namespace detail {

		template<typename Type>
		struct GetType
		{
		};

		template<>
		struct GetType<CBuffer::Camera>
		{
			static const Type value = Type::Camera;
		};

		template<>
		struct GetType<CBuffer::Transform>
		{
			static const Type value = Type::Transform;
		};

		template<>
		struct GetType<CBuffer::Light>
		{
			static const Type value = Type::Light;
		};
		template<>
		struct GetType<CBuffer::Bone>
		{
			static const Type value = Type::Bone;
		};
		template<>
		struct GetType<CBuffer::Environment>
		{
			static const Type value = Type::Environment;
		};
		template<>
		struct GetType<CBuffer::Material>
		{
			static const Type value = Type::Material;
		};
		template<>
		struct GetType<CBuffer::Materials>
		{
			static const Type value = Type::Materials;
		};
	}
}
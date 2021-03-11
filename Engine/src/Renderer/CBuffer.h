#pragma once

#include "Renderer/Texture.h"
#include "Common/Camera.h"
#include "Common/Light.h"
#include "Common/Material.h"

namespace Engine::CBuffer {

	enum class Type
	{
		Camera, CubeCamera, Transform, Bone, TFactor, DispatchInfo, ToneMapFactor, Gamma,
		Environment, Light, LightCam, Material, Materials, 
		TextureInform, None
	};

	struct Camera
	{
		mat4 View;
		mat4 Projection;
		vec3 Position;
		int padding;

		void Upload(Engine::Camera& other);
	};

	struct TFactor
	{
		vec4 Factor;
		void Upload(float factor);
	};

	struct CubeCamera
	{
		mat4 View[6];
		mat4 Projection;

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
		float InnerAng;
		float OuterAngRcp;
		float RangeRcp;
		int padding[3];

		void Upload(Engine::Light& other);
	};

	struct DispatchInfo
	{
		uvec4 info;
		void Upload(uvec4& other);
	};

	struct LightCam
	{
		mat4 LightView;
		mat4 LightProjection;

		void Upload(Engine::Light& other);
	};

	struct ToneMapFactor
	{
		float WhiteSqr;
		float MiddleGray;
		float AverageLum;
		float padding;

		void Upload(float* other);
	};

	struct Gamma
	{
		uvec4 GammaCorection;
		void Upload(uvec4& other);
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
		bool UseShadowMap;
		vec4 bias;

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

	struct TextureInform
	{
		float TextureWidth;
		float TextureHeight;
		vec2 padding;

		void Upload(const std::shared_ptr<Engine::Texture> other);
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
		struct GetType<CBuffer::CubeCamera>
		{
			static const Type value = Type::CubeCamera;
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
		struct GetType<CBuffer::LightCam>
		{
			static const Type value = Type::LightCam;
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
		template<>
		struct GetType<CBuffer::TextureInform>
		{
			static const Type value = Type::TextureInform;
		};
		template<>
		struct GetType<CBuffer::TFactor>
		{
			static const Type value = Type::TFactor;
		};
		template<>
		struct GetType<CBuffer::DispatchInfo>
		{
			static const Type value = Type::DispatchInfo;
		};
		template<>
		struct GetType<CBuffer::ToneMapFactor>
		{
			static const Type value = Type::ToneMapFactor;
		};
		template<>
		struct GetType<CBuffer::Gamma>
		{
			static const Type value = Type::Gamma;
		};
	}
}
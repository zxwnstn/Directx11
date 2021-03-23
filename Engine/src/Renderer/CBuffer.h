#pragma once

#include "Renderer/Texture.h"
#include "Common/Camera.h"
#include "Common/Light.h"
#include "Common/Material.h"

namespace Engine::CBuffer {

	enum class Type
	{
		Camera, CubeCamera, Transform, Bone, TFactor, DispatchInfo, ToneMapFactor, Gamma, SkyBoxInfo, ShadingData,
		Environment, Light, LightCam, LightColor, LightPos, Material, Materials, CascadedViewProj, Cascaded,
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

	struct Cascaded
	{
		mat4 ToShadowSpace;
		vec4 ToCascadeOffsetX;
		vec4 ToCascadeOffsetY;
		vec4 ToCascadeScale;

		void Upload(Engine::CascadedMatrix& matrix);
	};

	struct CascadedViewProj
	{
		mat4 ViewProj[3];

		void Upload(Engine::CascadedMatrix& matrix);
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
		float OuterAng;
		float RangeRcp;
		uvec3 padding;

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

	struct LightPos
	{
		vec4 pos;

		void Upload(Engine::Light& other);
	};

	struct LightColor
	{
		vec4 color;

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

	struct SkyBoxInfo
	{
		vec4 Color;
		void Upload(vec3& color);
	};

	struct Environment
	{
		mat4 WorldMatrix;
		vec4 Ambient;

		void Upload(const Engine::Environment& other);
	};

	struct ShadingData
	{
		uvec4 Data1; //x = Lighting, y = shadow, z = diffuse(lambert, half), w = specular(phong blinn) 
		uvec4 Data2; //x = lambert Pow factor, y = deffered blend factor,

		void Upload(const Engine::ShadingData& other);
	};

	struct Material
	{
		vec4 Ambient;
		vec4 Diffuse;
		vec4 Specular;
		vec4 Emissive;
		vec4 Fresnel;
		float Shiness;
		float Roughness;
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
		float Roughness[MAXIMUM_PART];
		float Metalic[MAXIMUM_PART];
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
		struct GetType<CBuffer::LightPos>
		{
			static const Type value = Type::LightPos;
		};
		template<>
		struct GetType<CBuffer::LightCam>
		{
			static const Type value = Type::LightCam;
		};
		template<>
		struct GetType<CBuffer::LightColor>
		{
			static const Type value = Type::LightColor;
		};
		template<>
		struct GetType<CBuffer::Cascaded>
		{
			static const Type value = Type::Cascaded;
		};
		template<>
		struct GetType<CBuffer::CascadedViewProj>
		{
			static const Type value = Type::CascadedViewProj;
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
		struct GetType<CBuffer::SkyBoxInfo>
		{
			static const Type value = Type::SkyBoxInfo;
		};
		template<>
		struct GetType<CBuffer::Gamma>
		{
			static const Type value = Type::Gamma;
		};
		template<>
		struct GetType<CBuffer::ShadingData>
		{
			static const Type value = Type::ShadingData;
		};

	}
}
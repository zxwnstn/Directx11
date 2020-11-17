#pragma once

#include "Common/Camera.h"
#include "Common/Light.h"
#include "Common/Material.h"

namespace Engine::CBuffer {

	enum class Type
	{
		Camera, Transform, Bone, 
		Environment, Light, Material,
		None
	};

	struct Camera
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMFLOAT4 Position;

		void Upload(Engine::Camera& other);
	};

	struct Transform
	{
		DirectX::XMMATRIX Translate;
		DirectX::XMMATRIX Rotate;
		DirectX::XMMATRIX Scale;

		void Upload(const Engine::Transform& other);
	};

	struct Light
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT4 Direction;
		DirectX::XMFLOAT4 Color;
		int type;
		int padding[3];

		void Upload(const Engine::Light& other);
	};

	struct Bone
	{
		DirectX::XMFLOAT4X4 SkinnedTransform[100];
		void Upload(DirectX::XMFLOAT4X4* skinnedTransform);
	};

	struct Environment
	{
		DirectX::XMFLOAT4 Ambient;

		void Upload(const Engine::Environment& other);
	};

	struct Material
	{
		int MMode;
		DirectX::XMFLOAT3 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT3 Emissive;
		DirectX::XMFLOAT3 Specular;
		DirectX::XMFLOAT3 Fresnel;
		float Shiness;
		int padding[2];

		void Upload(const Engine::Material& other);
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

	}
}
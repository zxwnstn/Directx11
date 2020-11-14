#pragma once

#include "Common/Transform.h"

namespace Engine::CBuffer {

	enum class Type
	{
		Camera, Transform, Light, Bone, None
	};

	struct Camera
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMFLOAT4 Position;

		//void Upload(const ::Camera& other);
		void Copy(const Camera& other);
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
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT3 Direction;
		float Power;

		void Copy(const Light& other);
	};

	struct Bone
	{
		DirectX::XMFLOAT4X4 SkinnedTransform[100];
		void Upload(DirectX::XMFLOAT4X4* skinnedTransform, uint32_t size);
	};

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
}
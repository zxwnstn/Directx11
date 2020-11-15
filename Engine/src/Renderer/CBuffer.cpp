#include "pch.h"

#include "CBuffer.h"

namespace Engine::CBuffer {

	void Light::Copy(const Light & other)
	{
		Ambient = other.Ambient;
		Diffuse = other.Diffuse;
		Specular = other.Specular;
		Direction = other.Direction;
		Power = other.Power;
	}

	void Transform::Upload(const Engine::Transform & other)
	{
		Translate = other.GetTranslate();
		Rotate = other.GetRotate();
		Scale = other.GetScale();
	}

	void Bone::Upload(DirectX::XMFLOAT4X4* other, uint32_t size)
	{
		memcpy(SkinnedTransform, other, size);
	}

	void Camera::Upload(Engine::Camera & other)
	{
		auto translate = other.GetTransform().GetTranslateValue();
		Position = DirectX::XMFLOAT4(translate.x, translate.y, translate.z, 1.0f);
		World = DirectX::XMMatrixIdentity();
		View = DirectX::XMMatrixTranspose(other.GetViewMatrix());
		Projection = DirectX::XMMatrixTranspose(other.GetProjectionMatrix());
	}

}

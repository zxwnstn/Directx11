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

	void Camera::Copy(const Camera & other)
	{
		World = DirectX::XMMatrixIdentity();
		View = other.View;
		Projection = other.Projection;
	}

	void Bone::Upload(DirectX::XMFLOAT4X4* other, uint32_t size)
	{
		memcpy(SkinnedTransform, other, size);
	}

}

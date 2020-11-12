#include "pch.h"

#include "CBuffer.h"

void CBuffer::Light::Copy(const Light & other)
{
	Ambient = other.Ambient;
	Diffuse = other.Diffuse;
	Specular = other.Specular;
	Direction = other.Direction;
	Power = other.Power;
}

void CBuffer::Transform::Upload(const ::Transform & other)
{
	Translate = other.GetTranslate();
	Rotate = other.GetRotate();
	Scale = other.GetScale();
}

//void CBuffer::Camera::Upload(const Camera & other)
//{
//	World = DirectX::XMMatrixIdentity();
//	View = other.View;
//	Projection = other.Projection;
//}

void CBuffer::Camera::Copy(const Camera & other)
{
	World = DirectX::XMMatrixIdentity();
	View = other.View;
	Projection = other.Projection;
}

void CBuffer::Bone::Upload(DirectX::XMFLOAT4X4* other, uint32_t size)
{
	memcpy(SkinnedTransform, other, size);
}

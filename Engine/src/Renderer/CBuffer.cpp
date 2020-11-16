#include "pch.h"

#include "CBuffer.h"
#include "File/SerializedData.h"

namespace Engine::CBuffer {

	void Transform::Upload(const Engine::Transform & other)
	{
		Translate = other.GetTranslate();
		Rotate = other.GetRotate();
		Scale = other.GetScale();
	}

	void Bone::Upload(DirectX::XMFLOAT4X4* other)
	{
		//Assume joints maximum size is 100
		memcpy(SkinnedTransform, other, MAXIMUM_JOINTS * sizeof(DirectX::XMFLOAT4X4));
	}

	void Camera::Upload(Engine::Camera & other)
	{
		auto translate = other.GetTransform().GetTranslateValue();
		Position = DirectX::XMFLOAT4(translate.x, translate.y, translate.z, 1.0f);
		World = DirectX::XMMatrixIdentity();
		View = DirectX::XMMatrixTranspose(other.GetViewMatrix());
		Projection = DirectX::XMMatrixTranspose(other.GetProjectionMatrix());
	}

	void Environment::Upload(const Engine::Environment & other)
	{
		Ambient = other.Ambient;
	}

	void Light::Upload(const Engine::Light & other)
	{
		type = other.m_Type;
		Color = { other.m_Color.x, other.m_Color.y, other.m_Color.z, 1.0f };
		Position = { 0.0f, 0.0f, 0.0f, 0.0f };
		Direction = { other.m_Direction.x, other.m_Direction.y, other.m_Direction.z, 0.0f };
		if (type == 0) // Directional
		{
			//Direction = other.m_Direction;
		}
		if (type == 1) // point
		{
			//Position = other.m_Transform.GetTranslateValue();
		}
		if (type == 2)
		{
			//Position = other.m_Transform.GetTranslateValue();
			//auto directionVector = other.m_Transform.GetFowardVector();
			//Direction.x = directionVector.m128_f32[0];
			//Direction.x = directionVector.m128_f32[1];
			//Direction.x = directionVector.m128_f32[2];
		}
	}

	void Material::Upload(const Engine::Material& other)
	{
		MMode = other.MMode;
		Ambient = other.Ambient;
		Diffuse = other.Diffuse;
		Specular = other.Specular;
		Fresnel = other.Fresnel;
		Emissive = other.Emissive;
		Roughness = other.Roughness;
	}

}

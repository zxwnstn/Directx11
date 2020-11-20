#include "CBuffer.h"
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
		Type = other.m_Type;
		Color = { other.m_Color.x, other.m_Color.y, other.m_Color.z, 1.0f };
		Position = { 0.0f, 0.0f, 0.0f, 0.0f };
		Intensity = other.m_Intensity;

		if (Type == 0) // Directional
		{
			Direction = { other.m_Direction.x, other.m_Direction.y, other.m_Direction.z, 0.0f };
		}
		if (Type == 1) // point
		{
			//Position = other.m_Transform.GetTranslateValue();
		}
		if (Type == 2)
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
		Ambient.x = other.Ambient.x;
		Ambient.y = other.Ambient.y;
		Ambient.z = other.Ambient.z;
		Ambient.w = 1.0f;
		Diffuse.x = other.Diffuse.x;
		Diffuse.y = other.Diffuse.y;
		Diffuse.z = other.Diffuse.z;
		Diffuse.w = 1.0f;
		Specular.x = other.Specular.x;
		Specular.y = other.Specular.y;
		Specular.z = other.Specular.z;
		Specular.w = 1.0f;
		Emissive.x = other.Emissive.x;
		Emissive.y = other.Emissive.y;
		Emissive.z = other.Emissive.z;
		Emissive.w = 1.0f;
		Fresnel.x = other.Fresnel.x;
		Fresnel.z = other.Fresnel.z;
		Fresnel.w = 1.0f;
		Fresnel.y = other.Fresnel.y;
		Shiness = other.Shiness;
		MMode = other.MMode;
	}

	void Materials::Upload(const Engine::MaterialSet & other)
	{
		auto& mat = other.Materials;
		size_t i;
		for (i = 0; i < mat.size(); ++i)
		{
			auto curMat = mat.find(int(i))->second;
			MMode[i] = 7;
			Shiness[i] = curMat.Shiness;

			Ambient[i].x = curMat.Ambient.x;
			Diffuse[i].x = curMat.Diffuse.x;
			Diffuse[i].y = curMat.Diffuse.y;
			Diffuse[i].z = curMat.Diffuse.z;
			Diffuse[i].w = 1.0f;
			Specular[i].x = curMat.Specular.x;
			Specular[i].y = curMat.Specular.y;
			Specular[i].z = curMat.Specular.z;
			Specular[i].w = 1.0f;
			Emissive[i].x = curMat.Emissive.x;
			Emissive[i].y = curMat.Emissive.y;
			Emissive[i].z = curMat.Emissive.z;
			Emissive[i].w = 1.0f;
			Fresnel[i].x = curMat.Fresnel.x;
			Fresnel[i].y = curMat.Fresnel.y;
			Fresnel[i].z = curMat.Fresnel.z;
			Fresnel[i].w = 1.0f;
		}
	}

	

}


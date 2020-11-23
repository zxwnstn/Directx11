#include "CBuffer.h"
#include "pch.h"

#include "CBuffer.h"

namespace Engine::CBuffer {

	void Transform::Upload(Engine::Transform & other)
	{
		Scale = Util::Scale(other.GetScale(), true);
		Rotate = Util::Rotate(other.GetRotate(), true);
		Translate = Util::Translate(other.GetTranslate(), true);
	}

	void Camera::Upload(Engine::Camera & other)
	{
		auto translate = other.GetTransform();
		Position = other.GetTransform().GetTranslate();
		View = Util::Transpose(other.GetViewMatrix());
		Projection = Util::Transpose(other.GetProjectionMatrix());
	}

	void Environment::Upload(const Engine::Environment & other)
	{
		Ambient = other.Ambient;
		WorldMatrix = other.WorldMatrix;
	}

	void Light::Upload(const Engine::Light & other)
	{
		Type = other.m_Type;
		Color = other.m_Color;
		auto pos = other.m_Transform.GetTranslate();
		Position.x = pos.x;
		Position.y = pos.y;
		Position.z = pos.z;
		Position.w = 1.0f;

		Intensity = other.m_Intensity;

		if (Type == 0) // Directional
		{
			Direction = other.m_Direction;
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
		Ambient = other.Ambient;
		Diffuse = other.Diffuse;
		Specular = other.Specular;
		Emissive = other.Emissive;
		Fresnel = other.Fresnel;
		Shiness = other.Shiness;
		MapMode = other.MapMode;
	}

	void Materials::Upload(const Engine::MaterialSet & other)
	{
		auto& mat = other.Materials;
		size_t i;
		for (i = 0; i < mat.size(); ++i)
		{
			auto curMaterial = mat.find(int(i))->second;

			Ambient[i] = curMaterial.Ambient;
			Diffuse[i] = curMaterial.Diffuse;
			Specular[i] = curMaterial.Specular;
			Emissive[i] = curMaterial.Emissive;
			Fresnel[i] = curMaterial.Fresnel;

			MMode[i] = curMaterial.MapMode;
			Shiness[i] = curMaterial.Shiness;
		}
	}

	void Bone::Upload(mat4 * skinnedTransform)
	{
		memcpy(SkinnedTransform, skinnedTransform, sizeof(mat4) * MAXIMUM_JOINTS);
	}

}


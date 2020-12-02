#include "pch.h"

#include "CBuffer.h"
#include "Renderer/Texture.h"

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
		UseShadowMap = other.UseShadowMap;
		bias = other.bias;
	}

	void Light::Upload(Engine::Light & other)
	{
		auto& transform = other.lightCam.GetTransform();

		auto lookat = transform.GetLookAtVector();
		Direction.x = lookat.x;
		Direction.y = lookat.y;
		Direction.z = lookat.z;
		Direction.w = 0.0f;

		Color = other.m_Color;
		Intensity = other.m_Intensity;
		Type = other.m_Type;
		RangeRcp = 1.0f / other.m_Range;

		InnerAng = other.m_InnerAngle;
		OuterAngRcp = 1.0f / other.m_OuterAngle;
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

	void Light2::Upload(Engine::Light & other)
	{
		LightView = Util::Transpose(other.lightCam.GetViewMatrix());
		LightProjection = Util::Transpose(other.lightCam.GetProjectionMatrix());
	}

	void TextureInform::Upload(const std::shared_ptr<Engine::Texture> other)
	{
		TextureWidth = (float)other->Width;
		TextureHeight = (float)other->Height;
	}

	void LightPos::Upload(Engine::Light & other)
	{
		auto translate = other.lightCam.GetTransform().GetTranslate();
		lightPos.x = translate.x;
		lightPos.y = translate.y;
		lightPos.z = translate.z;
		lightPos.w = 1.0f;
	}

}


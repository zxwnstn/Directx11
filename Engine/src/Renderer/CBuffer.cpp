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
	}

	void Light::Upload(Engine::Light & other)
	{
		auto& transform = other.lightCam.GetTransform();

		auto pos = transform.GetTranslate();
		Position.x = pos.x;
		Position.y = pos.y;
		Position.z = pos.z;
		Position.w = 1.0f;

		auto lookat = transform.GetLookAtVector();
		Direction.x = lookat.x;
		Direction.y = lookat.y;
		Direction.z = lookat.z;
		Direction.w = 0.0f;

		Color = other.m_Color;
		Intensity = other.m_Intensity;
		Type = other.m_Type;
		RangeRcp = 1.0f / other.m_Range;

		InnerAng = cosf(other.m_InnerAngle);
		OuterAng = cosf(other.m_OuterAngle);
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
		Roughness = other.Roughness;
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
			Roughness[i] = curMaterial.Roughness;
			Metalic[i] = curMaterial.Metalic;
		}
	}

	void Bone::Upload(mat4 * skinnedTransform)
	{
		memcpy(SkinnedTransform, skinnedTransform, sizeof(mat4) * MAXIMUM_JOINTS);
	}

	void LightCam::Upload(Engine::Light & other)
	{

		auto transform = other.lightCam.GetTransform();
		LightView = Util::Transpose(other.lightCam.GetViewMatrix());
		LightProjection = Util::Transpose(other.lightCam.GetProjectionMatrix());
	}

	void TextureInform::Upload(const std::shared_ptr<Engine::Texture> other)
	{
		TextureWidth = (float)other->Width;
		TextureHeight = (float)other->Height;
	}

	void CubeCamera::Upload(Engine::Camera & other)
	{
		for (int i = 0; i < 6; ++i)
			View[i] = Util::Transpose(Util::GetCubeViewMatrix(other.GetTransform().GetTranslate(), i));
		Projection = Util::Transpose(other.GetProjectionMatrix());
	}

	void TFactor::Upload(float factor)
	{
		Factor.x = factor;
	}

	void DispatchInfo::Upload(uvec4& other)
	{
		info = other;
	}

	void ToneMapFactor::Upload(float * other)
	{
		WhiteSqr = other[0] * other[0];
		MiddleGray = other[1];
		AverageLum = other[2];
		padding = 0.0f;
	}

	void Gamma::Upload(uvec4 & other)
	{
		GammaCorection = other;
	}

	void LightPos::Upload(Engine::Light & other)
	{
		auto translate = other.lightCam.GetTransform().GetTranslate();
		pos.x = translate.x;
		pos.y = translate.y;
		pos.z = translate.z;
	}

	void LightColor::Upload(Engine::Light & other)
	{
		color = other.m_Color;
		color.w = 1.0f;
	}

	void CascadedViewProj::Upload(Engine::CascadedMatrix& matrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			ViewProj[i] = Util::Transpose(matrix.m_arrWorldToCascadeProj[i]);
		}
	}

	void Cascaded::Upload(Engine::CascadedMatrix& matrix)
	{
		ToShadowSpace = Util::Transpose(matrix.m_WorldToShadowSpace);
		ToCascadeOffsetX = matrix.m_vToCascadeOffsetX;
		ToCascadeOffsetY = matrix.m_vToCascadeOffsetY;
		ToCascadeScale = matrix.m_vToCascadeScale;
	}

	void SkyBoxInfo::Upload(vec3 & color)
	{
		Color.x = color.x;
		Color.y = color.y;
		Color.z = color.z;
		Color.w = 1.0f;
	}

	void ShadingData::Upload(const Engine::ShadingData & other)
	{
		Data1 = other.Data1;
		Data2 = other.Data2;
	}

}


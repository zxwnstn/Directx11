#include "pch.h"

#include "Model.h"

#include "Renderer/ModelBuffer.h"
#include "Common/Transform.h"
#include "Renderer/Texture.h"
#include "3D/SkeletalAnimation.h"
#include "3D/Skeleton.h"

namespace Engine {

	Model2D::Model2D(const std::string & shaderName)
		: m_Shader(shaderName)
	{
	}

	ModelBuilder2D Model2D::Create(RenderingShader type, std::string&& shaderName)
	{
		if (shaderName.empty())
		{
			ASSERT(type != RenderingShader::Custom, "Custom shader must specify shader name");
			shaderName = ToString(type);
		}
		return ModelBuilder2D(new Model2D(shaderName));
	}

	void Model2D::Update(float dt)
	{
	}


	Model3D::Model3D(const std::string & ShaderName)
		: m_Shader(ShaderName)
	{

	}

	void Model3D::Update(float dt)
	{
		animationUpdate(dt);
	}

	bool Model3D::SetAnimation(const std::string & animationName, bool loop)
	{
		auto find = std::find(m_Animation->AnimList.begin(), m_Animation->AnimList.end(), animationName);
		if (find == m_Animation->AnimList.end())
			return false;

		m_Animation->CurAnim = animationName;
		m_Animation->Loop = loop;
		m_Animation->Duration = SkeletalAnimationArchive::GetAnimationDuration(m_Skeleton->Name, m_Animation->CurAnim);
		m_Animation->KeyInterval = SkeletalAnimationArchive::GetKeyInterval(m_Skeleton->Name, m_Animation->CurAnim);

		return true;
	}

	ModelBuilder Model3D::Create(RenderingShader type, std::string && shaderName)
	{
		if (shaderName.empty())
		{
			ASSERT(type != RenderingShader::Custom, "Custom shader must specify shader name");
			shaderName = ToString(type);
		}
		return ModelBuilder(new Model3D(shaderName));
	}

	void Model3D::SetShader(const std::string& shader)
	{
		m_Shader = shader;
	}

	void Model3D::animationUpdate(float dt)
	{
		if (!m_Animation->Expired)
			m_Animation->Elapsedtime += dt;

		if (m_Animation->Elapsedtime >= m_Animation->Duration)
		{
			if (m_Animation->Loop)
			{
				m_Animation->Elapsedtime = 0.0f;
			}
			else
			{
				m_Animation->Expired = true;
			}
		}

		SkeletalAnimationPlayer::Play(m_Skeleton->Name, m_Animation);
	}

	
}

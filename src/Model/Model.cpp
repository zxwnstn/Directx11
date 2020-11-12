#include "pch.h"

#include "Model.h"

#include "3D/ModelBuilder.h"

#include "Renderer/ModelBuffer.h"
#include "Common/Transform.h"
#include "Renderer/Texture.h"
#include "3D/SkeletalAnimation.h"

void Model3D::Bind()
{
}

Model3D::Model3D(const std::string & ShaderName)
	: m_Shader(ShaderName)
{
	
}

void Model3D::Update(float dt)
{
	SkeletalAnimationPlayer p; p.Play(m_Animation);
}

ModelBuilder Model3D::Create(RenderingShader type, std::string && ShaderName)
{
	if (ShaderName.empty())
	{
		assert(type != RenderingShader::Custum);
		ShaderName = ToString(type);
	}
	return ModelBuilder(new Model3D(ShaderName));
}


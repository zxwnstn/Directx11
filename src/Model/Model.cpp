#include "pch.h"

#include "Model.h"

#include "Renderer/ModelBuffer.h"
#include "3D/Skeleton.h"
#include "3D/SkeletalAnimation.h"
#include "Common/Transform.h"
#include "Texture.h"

void Model3D::Bind()
{
}

void Model3D::Update(float dt)
{
	/*
	
	*/

	SkeletalAnimationPlayer::Play(m_Animation);
}

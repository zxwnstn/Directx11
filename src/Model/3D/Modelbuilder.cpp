#include "pch.h"

#include "ModelBuilder.h"
#include "Model/Model.h"
#include "Skeleton.h"
#include "SkeletalAnimation.h"
#include "Renderer/ModelBuffer.h"
#include "Renderer/Texture.h"

/***************************************/
/********  finalModel Builder  *********/
/***************************************/

FinalModelBuilder::FinalModelBuilder(Model3D * myModel)
	: myModel(myModel)
{
}

/***************************************/
/********  FbxModel Builder  ***********/
/***************************************/
FbxModelBuilder::FbxModelBuilder(Model3D * myModel)
	: myModel(myModel)
{}

FinalModelBuilder FbxModelBuilder::SetSkeleton(const std::string & skeletonName)
{
	myModel->m_Skeleton = SkeletonArchive::Get(skeletonName);
	myModel->m_ModelBuffer = Renderer::GetShader(myModel->m_Shader)
		.CreateCompotibleBuffer()
		.SetBuffer(myModel->m_Skeleton->Vertices.data(), myModel->m_Skeleton->Indices.data(), (uint32_t)myModel->m_Skeleton->Indices.size());

	myModel->m_Textures.push_back(TextureArchive::Get(skeletonName + "_diffuse"));
	myModel->m_Textures.push_back(TextureArchive::Get(skeletonName + "_normal"));
	myModel->m_Textures.push_back(TextureArchive::Get(skeletonName + "_specular"));

	myModel->m_Animation.reset(new AnimationInform);

	auto& animList = SkeletalAnimationArchive::GetAnimList(skeletonName);
	myModel->m_Animation->AnimList = animList;
	myModel->SetAnimation(myModel->m_Animation->AnimList[0], true);

	return FinalModelBuilder(myModel);
}



/***************************************/
/*******  NoneFbxModel Builder  ********/
/***************************************/
NoneFbxModelBuilder::NoneFbxModelBuilder(Model3D * myModel)
	: myModel(myModel)
{}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetIndices()
{
	return *this;
}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetVertices()
{
	return *this;
}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetTexture()
{
	return *this;
}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetMaterial()
{
	return *this;
}

FinalModelBuilder NoneFbxModelBuilder::Finish()
{
	return FinalModelBuilder(myModel);
}

/***************************************/
/*********  Model Builder  *************/
/***************************************/
ModelBuilder::ModelBuilder(Model3D * myModel)
	: myModel(myModel)
{}

FbxModelBuilder ModelBuilder::buildFromFBX()
{
	return FbxModelBuilder(myModel);
}

NoneFbxModelBuilder ModelBuilder::buildCustum()
{
	return NoneFbxModelBuilder(myModel);
}


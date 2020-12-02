#include "pch.h"

#include "ModelBuilder.h"
#include "Model/Model.h"
#include "Skeleton.h"
#include "SkeletalAnimation.h"
#include "Renderer/ModelBuffer.h"
#include "Renderer/Texture.h"

namespace Engine {

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
		if (!myModel->m_Skeleton)
		{
			LOG_WARN("There is no Skeleton : {0}", skeletonName);
			delete myModel;
			return FinalModelBuilder(nullptr);
		}

		myModel->m_ModelBuffer = Renderer::GetShader(myModel->m_Shader)
			.CreateCompotibleBuffer()
			.SetMesh(MeshArchive::GetSkeletalMesh(skeletonName));

		myModel->m_Texture = TextureArchive::Get(skeletonName);
		myModel->m_MaterialSet = MaterialArchive::GetSet(skeletonName);
		myModel->m_Animation.reset(new AnimationInform);

		auto& animList = SkeletalAnimationArchive::GetAnimList(skeletonName);
		myModel->m_Animation->AnimList = animList;

		if (myModel->m_Animation->AnimList.empty())
		{
			LOG_WARN("There is no any Animations : {0}", skeletonName);
			return FinalModelBuilder(myModel);
		}


		myModel->SetAnimation(myModel->m_Animation->AnimList[0], true);

		return FinalModelBuilder(myModel);
	}

	/***************************************/
	/*******  NoneFbxModel Builder  ********/
	/***************************************/
	NoneFbxModelBuilder::NoneFbxModelBuilder(Model3D * myModel)
		: myModel(myModel)
	{}

	NoneFbxModelBuilder & NoneFbxModelBuilder::SetMesh(const std::string & meshName)
	{
		myModel->m_ModelBuffer = Renderer::GetShader(myModel->m_Shader)
			.CreateCompotibleBuffer()
			.SetMesh(MeshArchive::GetStaticMesh(meshName));
		return *this;
	}

	NoneFbxModelBuilder & NoneFbxModelBuilder::SetTexture(const std::string & textureName)
	{
		myModel->m_Texture = TextureArchive::Get(textureName);
		return *this;
	}

	NoneFbxModelBuilder & NoneFbxModelBuilder::SetMaterial(const std::string & materialName)
	{
		myModel->m_MaterialSet = MaterialArchive::GetSet(materialName);
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

	StaticModelBuilder ModelBuilder::buildFromOBJ()
	{
		return StaticModelBuilder(myModel);
	}

	StaticModelBuilder::StaticModelBuilder(Model3D * myModel)
		: myModel(myModel)
	{
	}

	FinalModelBuilder StaticModelBuilder::SetObject(const std::string & objectName)
	{
		myModel->m_ModelBuffer = Renderer::GetShader(myModel->m_Shader)
			.CreateCompotibleBuffer()
			.SetMesh(MeshArchive::GetStaticMesh(objectName));

		myModel->m_Texture = TextureArchive::Get(objectName);
		myModel->m_MaterialSet = MaterialArchive::GetSet(objectName);


		return FinalModelBuilder(myModel);
	}

}


#include "pch.h"

#include "ModelBuilder.h"
#include "Model/Model.h"
#include "Skeleton.h"
#include "SkeletalAnimation.h"
#include "Renderer/ModelBuffer.h"
#include "Renderer/Texture.h"
#include "Common/Material.h"

namespace Engine {

	/***************************************/
	/********  finalModel Builder  *********/
	/***************************************/
	FinalModelBuilder::FinalModelBuilder(Model3D * myModel)
		: myModel(myModel)
	{}

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
		myModel->m_MeshName = skeletonName;

		myModel->m_ModelBuffer = ModelBuffer::Create(MeshType::Skeletal)
			.SetMesh(MeshArchive::GetSkeletalMesh(skeletonName));

		myModel->m_MaterialSet = MaterialArchive::GetSetCopy(skeletonName);
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
	/*******  ObjModel Builder  ********/
	/***************************************/
	ObjModelBuilder::ObjModelBuilder(Model3D * myModel)
		: myModel(myModel)
	{
	}

	FinalModelBuilder ObjModelBuilder::SetObject(const std::string & objectName)
	{
		MeshType type = MeshType::Static;
		if (objectName == "SkyBox")
			type = MeshType::SkyBox;
		myModel->m_ModelBuffer = ModelBuffer::Create(type)
			.SetMesh(MeshArchive::GetStaticMesh(objectName));

		myModel->m_MeshName = objectName;

		myModel->m_MaterialSet = MaterialArchive::GetSetCopy(objectName);

		return FinalModelBuilder(myModel);
	}

	/***************************************/
	/*******  CustomModel Builder  ********/
	/***************************************/
	CustomModelBuilder::CustomModelBuilder(Model3D * myModel)
		: myModel(myModel)
	{}

	CustomModelBuilder & CustomModelBuilder::SetMesh(const std::string & meshName)
	{
		myModel->m_ModelBuffer = ModelBuffer::Create(MeshType::Static)
			.SetMesh(MeshArchive::GetStaticMesh(meshName));
		return *this;
	}

	CustomModelBuilder & CustomModelBuilder::SetMaterial(const std::string & materialName)
	{
		myModel->m_MaterialSet = MaterialArchive::GetSet(materialName);
		return *this;
	}
	
	FinalModelBuilder CustomModelBuilder::Finish()
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

	CustomModelBuilder ModelBuilder::buildCustum()
	{
		return CustomModelBuilder(myModel);
	}

	ObjModelBuilder ModelBuilder::buildFromOBJ()
	{
		return ObjModelBuilder(myModel);
	}

}


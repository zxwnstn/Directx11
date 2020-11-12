#include "pch.h"

#include <fbxsdk.h>
#include "FbxLoader.h"
#include "FileCommon.h"

#include "Model/3D/Skeleton.h"
#include "Model/3D/SkeletalAnimation.h"


void ControlPoint::push(float weight, uint32_t index)
{
	BoneWeight.m[i] = weight;
	BoneIndex.m[i] = index;
	++i;
}

FBXLoader::FBXLoader(const std::string & directorName)
{
	m_Manager = FbxManager::Create();
	m_Manager->SetIOSettings(FbxIOSettings::Create(m_Manager, IOSROOT));

	data.skeleton = new Skeleton;
	data.skeleton->Name = directorName;

	if (isExistCache(directorName)) isCached = true;
}

FBXLoader::~FBXLoader()
{
	delete[] data.ControlPoints;
	m_Manager->Destroy();
}

void FBXLoader::import(const std::string & filename)
{
	importer = FbxImporter::Create(m_Manager, "");
	scene = FbxScene::Create(m_Manager, "");
	importer->Initialize(filename.c_str(), -1, m_Manager->GetIOSettings());
	importer->Import(scene);
	root = scene->GetRootNode();
}

void FBXLoader::init(const std::string& filename)
{
	import(filename);

	FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::MayaYUp.ConvertScene(scene);
	FbxGeometryConverter geometryConverter(m_Manager);
	geometryConverter.Triangulate(scene, true);

	isLoaded = true;
}

void FBXLoader::resetScene()
{
	importer->Destroy();
	scene->Destroy();
}


void FBXLoader::extractAll()
{
	auto count = root->GetChildCount();
	for (int i = 0; i < count; ++i)
	{
		FbxNode* node = root->GetChild(i);
		FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();

		switch (nodeType)
		{
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			getSkeleton(root);
			break;
		case fbxsdk::FbxNodeAttribute::eMesh:
			getControlPoint(node);
			getAnimation(node);
			getVertices(node);
			//procMaterial(node, result);
			break;
		}
	}
}



void FBXLoader::Extract(const std::string & path, const std::string & file)
{
	filename = file;
	if (isLoaded)
	{
		import(path + file);
		
		auto count = root->GetChildCount();
		for (int i = 0; i < count; ++i)
		{
			FbxNode* node = root->GetChild(i);
			FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();

			switch (nodeType)
			{
			case fbxsdk::FbxNodeAttribute::eMesh:
				getAnimation(node);
				break;
			}
		}
	}
	else
	{
		init(path + file);
		extractAll();
	}
	resetScene();
}

bool FBXLoader::isExistCache(const std::string& file)
{
	return false;
}

void FBXLoader::getControlPoint(FbxNode* node)
{
	FbxMesh* mesh = node->GetMesh();
	uint32_t count = mesh->GetControlPointsCount();

	data.ControlPoints = new ControlPoint[count];
	data.ControlPointCount = count;
	for (uint32_t i = 0; i < count; ++i)
	{
		for (uint32_t j = 0; j < 3; ++j)
		{
			data.ControlPoints[i].Position.m[j] = static_cast<float>(mesh->GetControlPointAt(i).mData[j]);
		}
	}
}

void FBXLoader::getVertices(FbxNode* node)
{
	FbxMesh* mesh = node->GetMesh();
	int count = mesh->GetPolygonCount();
	uint32_t vertexCount = count * 3;

	data.Vertices = new Vertex[vertexCount];
	data.Indices = new uint32_t[vertexCount];

	uint32_t index = 0;
	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int controlIndex = mesh->GetPolygonVertex(i, j);
			data.Vertices[index].Position = data.ControlPoints[controlIndex].Position;
			data.Vertices[index].UV = procUV(mesh, index, controlIndex);

			for (int k = 0; k < 4; ++k)
			{
				data.Vertices[index].BoneIndex.m[k] = data.ControlPoints[controlIndex].BoneIndex.m[k];
				data.Vertices[index].BoneWeight.m[k] = data.ControlPoints[controlIndex].BoneWeight.m[k];
			}
			data.Indices[index] = index;

			index++;
		}
	}
	data.skeleton->Vertices = data.Vertices;
	data.skeleton->Indices = data.Indices;
	data.skeleton->VerticesCount = data.VerticesCount;
	SkeletonArchive::Add(data.skeleton);
}


void FBXLoader::createHierachy(FbxNode* node, int index, int parent)
{
	FbxNodeAttribute::EType type = node->GetNodeAttribute()->GetAttributeType();
	if (type == FbxNodeAttribute::EType::eSkeleton)
	{
		Joint joint;
		joint.Parent = parent;
		joint.Name = std::string(node->GetName());
		data.skeleton->Joints.push_back(joint);
	}
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		createHierachy(node->GetChild(i), (int)data.skeleton->Joints.size(), index);
	}

}

void FBXLoader::getSkeleton(FbxNode* node)
{	
	for (int child = 0; child < node->GetChildCount(); ++child)
	{
		FbxNode* cur = node->GetChild(child);
		createHierachy(cur, 0, -1);
	}
	data.skeleton->NumJoint = (uint32_t)data.skeleton->Joints.size();
}

void FBXLoader::getAnimation(FbxNode* node)
{
	//Defomer -> cluster -> link(Joint)
	FbxMesh* mesh = node->GetMesh();
	unsigned int numOfDeformers = mesh->GetDeformerCount();
	FbxAMatrix geometryTransform;
	geometryTransform.SetIdentity();

	data.Animation = new SkeletalAnimtion;

	//Defomer
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!skin) continue;

		unsigned int numOfClusters = skin->GetClusterCount();
		data.Animation->JointAnimations.resize(numOfClusters);

		//for each Cluster (that contains link)
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);
			

			//0. Get JointIdex
			std::string jointName = cluster->GetLink()->GetName();
			unsigned int jointIndex;
			for (jointIndex = 0; jointIndex < data.skeleton->Joints.size(); ++jointIndex)
				if (jointName == data.skeleton->Joints[jointIndex].Name)
					break;

			//1. Calc OffsetMat
			{
				FbxAMatrix transformMatrix;
				FbxAMatrix transformLinkMatrix;
				FbxAMatrix offsetMat;

				//월드상에서 조인트의 Transform이다.(JointTransform)
				//조인트 트랜스폼의 역행렬을 취해줌 으로서 해당매트릭스의 포지션으로 가게된다.
				DirectX::XMFLOAT4X4 offset;

				cluster->GetTransformMatrix(transformMatrix);
				cluster->GetTransformLinkMatrix(transformLinkMatrix);
				offsetMat = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						offset.m[i][j] = (float)offsetMat[i][j];
					}
				}
				data.skeleton->Joints[jointIndex].Offset = DirectX::XMLoadFloat4x4(&offset);
			}
			

			//Get Joint Weight
			{
				unsigned int indices = cluster->GetControlPointIndicesCount();
				auto controlPointIndices = cluster->GetControlPointIndices();
				for (unsigned int i = 0; i < indices; ++i)
				{
					data.ControlPoints[controlPointIndices[i]].push((float)cluster->GetControlPointWeights()[i], jointIndex);
				}
			}
			
			//Anim
			//Todo : Supply multiple Animation of one fbx file
			{
				FbxTime::EMode frameRate = scene->GetGlobalSettings().GetTimeMode();

				FbxTakeInfo* takeInfo = importer->GetTakeInfo(0);
				std::string takeName = takeInfo->mName.Buffer();

				FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
				FbxTime stop = takeInfo->mLocalTimeSpan.GetStop();
				data.Animation->JointAnimations[clusterIndex].Duration = (float)(stop - start).GetSecondDouble();

				for (uint32_t timePoint = 0; timePoint <= stop.GetFrameCount(frameRate); ++timePoint)
				{
					FbxTime curTime;
					KeyFrame keyFrame;

					curTime.SetFrame(timePoint, frameRate);
					keyFrame.Start = (float)curTime.GetSecondDouble();

					FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(curTime) * geometryTransform;
					auto globalTransform = currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(curTime);

					FbxVector4 TS = globalTransform.GetT();
					keyFrame.Translation = {
						static_cast<float>(TS.mData[0]),
						static_cast<float>(TS.mData[1]),
						static_cast<float>(TS.mData[2]),
					};
					TS = globalTransform.GetS();
					keyFrame.Scale = {
						static_cast<float>(TS.mData[0]),
						static_cast<float>(TS.mData[1]),
						static_cast<float>(TS.mData[2]) };
					FbxQuaternion Q = globalTransform.GetQ();
					keyFrame.RotationQuat = {
						static_cast<float>(Q.mData[0]),
						static_cast<float>(Q.mData[1]),
						static_cast<float>(Q.mData[2]) ,
						static_cast<float>(Q.mData[3]) };

					data.Animation->JointAnimations[clusterIndex].KeyFrames[timePoint] = keyFrame;
				}
			}
		}
	}

	SkeletalAnimationArchive::Add(data.skeleton->Name, filename, data.Animation);
}


vec2 FBXLoader::procUV(FbxMesh* mesh, int index, int controlIndex)
{
	if (mesh->GetElementNormalCount() < 1)
		return vec2{ 0,0 };

	vec2 ret;

	auto uv = mesh->GetElementUV();
	auto mapMode = uv->GetMappingMode();
	auto refMode = uv->GetReferenceMode();

	switch (mapMode)
	{
	case fbxsdk::FbxLayerElement::eByControlPoint:
		switch (refMode)
		{
		case fbxsdk::FbxLayerElement::eDirect:
			ret.m[0] = static_cast<float>(uv->GetDirectArray().GetAt(controlIndex).mData[0]);
			ret.m[1] = static_cast<float>(uv->GetDirectArray().GetAt(controlIndex).mData[1]);
			break;
		case fbxsdk::FbxLayerElement::eIndexToDirect:
			auto itd = uv->GetIndexArray().GetAt(controlIndex);
			ret.m[0] = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[0]);
			ret.m[1] = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[1]);
			break;
		}
		break;
	case fbxsdk::FbxLayerElement::eByPolygonVertex:
		switch (refMode)
		{
		case fbxsdk::FbxLayerElement::eDirect:
			ret.m[0] = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[0]);
			ret.m[1] = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[1]);
			break;
		case fbxsdk::FbxLayerElement::eIndexToDirect:
			auto itd = uv->GetIndexArray().GetAt(index);
			ret.m[0] = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[0]);
			ret.m[1] = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[1]);
			break;
		}
		break;
	}

	return ret;
}

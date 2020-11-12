#include "pch.h"

#include <fbxsdk.h>
#include "FbxLoader2.h"
#include "FileCommon.h"



FBXLoader::FBXLoader(const std::string & file)
{
	if (!File::isExistFile(file)) 
		return;

	m_Manager = FbxManager::Create();
	m_Manager->SetIOSettings(FbxIOSettings::Create(m_Manager, IOSROOT));

	if (isExistCache(file))
	{
		isCached = true;
		return;
	}

	importer = FbxImporter::Create(m_Manager, "");
	scene = FbxScene::Create(m_Manager, "");
	importer->Initialize(file.c_str(), -1, m_Manager->GetIOSettings());
	importer->Import(scene);
}

void FBXLoader::Extract(uint64_t flag)
{
	auto count = root->GetChildCount();
	for (int i = 0; i < count; ++i)
	{
		FbxNode* node = root->GetChild(i);
		FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();

		switch (nodeType)
		{
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			getBoneHierachy();
			break;
		case fbxsdk::FbxNodeAttribute::eMesh:
			getControlPoint(node);
			getAnimation();
			getVertices(node);
			getMaterial();
			break;
		}
	}
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
}


void FBXLoader::createHierachy(FbxNode* node, int index, int parent)
{
	FbxNodeAttribute::EType type = node->GetNodeAttribute()->GetAttributeType();
	if (type == FbxNodeAttribute::EType::eSkeleton)
	{
		Joint joint;
		joint.Parent = parent;
		joint.Name = std::string(node->GetName());
		ret.skeleton.Joints.push_back(joint);
	}
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		createHierachy(node->GetChild(i), ret, ret.skeleton.Joints.size(), index);
	}

}

void FBXLoader::getBoneHierachy(FbxNode* node)
{
	for (int child = 0; child < node->GetChildCount(); ++child)
	{
		FbxNode* cur = node->GetChild(child);
		createHierachy(cur, ret, 0, -1);
	}
}

void FBXLoader::getAnimation(FbxNode* node)
{
	//Defomer -> cluster -> link(Joint)
	FbxMesh* mesh = node->GetMesh();
	unsigned int numOfDeformers = mesh->GetDeformerCount();
	FbxAMatrix geometryTransform;
	geometryTransform.SetIdentity();

	//Defomer
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!skin) continue;

		unsigned int numOfClusters = skin->GetClusterCount();
		//for each Cluster (that contains link)
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);

			//0. Get JointIdex
			std::string jointName = cluster->GetLink()->GetName();
			unsigned int jointIndex;
			for (jointIndex = 0; jointIndex < ret.skeleton.Joints.size(); ++jointIndex)
				if (jointName == ret.skeleton.Joints[jointIndex].Name)
					break;

			//1. Calc OffsetMat
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix offsetMat;
			DirectX::XMFLOAT4X4 _offsetMat;
			//축정렬이 되었다면 단위행렬
			//월드상에서 조인트의 Transform이다.(JointTransform)
			//조인트 트랜스폼의 역행렬을 취해줌 으로서 해당매트릭스의 포지션으로 가게된다.
			cluster->GetTransformMatrix(transformMatrix);
			cluster->GetTransformLinkMatrix(transformLinkMatrix);
			offsetMat = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					_offsetMat.m[i][j] = offsetMat[i][j];
				}
			}
			ret.skeleton.Joints[jointIndex].OffsetMat = _offsetMat;

			//Get Bone Weight
			unsigned int indices = cluster->GetControlPointIndicesCount();
			auto controlPointIndices = cluster->GetControlPointIndices();
			for (unsigned int i = 0; i < indices; ++i)
			{
				BoneWeight boneWeight;
				boneWeight.BoneIndex = jointIndex;
				boneWeight.Weight = cluster->GetControlPointWeights()[i];

				ret.ControlPoints[controlPointIndices[i]].BoneWeights.push_back(boneWeight);
				ret.ControlPoints[controlPointIndices[i]].boneName = jointName;
			}

			//Anim
			//Todo : Supply multiple Animation of one fbx file
			FbxTime::EMode frameRate = scene->GetGlobalSettings().GetTimeMode();
			int animStackCount = m_Importer->GetAnimStackCount();

			FbxTakeInfo* takeInfo = m_Importer->GetTakeInfo(0);
			std::string takeName = takeInfo->mName.Buffer();

			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime stop = takeInfo->mLocalTimeSpan.GetStop();

			for (FbxLongLong j = 0; j <= stop.GetFrameCount(frameRate); ++j)
			{
				FbxTime curTime;
				KeyFrame keyFrame;

				curTime.SetFrame(j, frameRate);
				keyFrame.start = curTime.GetSecondDouble();

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
				curTime.SetFrame(j + 1, frameRate);
				keyFrame.end = curTime.GetSecondDouble();

				ret.skeleton.Joints[clusterIndex].Anim[takeName].KeyFrames.push_back(keyFrame);
			}
		}
	}

	BoneWeight boneWeight;
	boneWeight.Weight = 0;
	boneWeight.BoneIndex = 0;
	for (auto itr = ret.ControlPoints.begin(); itr != ret.ControlPoints.end(); ++itr)
	{
		for (unsigned int i = itr->BoneWeights.size(); i < 4; ++i)
		{
			itr->BoneWeights.push_back(boneWeight);
		}
	}

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
			ret.x = static_cast<float>(uv->GetDirectArray().GetAt(controlIndex).mData[0]);
			ret.y = static_cast<float>(uv->GetDirectArray().GetAt(controlIndex).mData[1]);
			break;
		case fbxsdk::FbxLayerElement::eIndexToDirect:
			auto itd = uv->GetIndexArray().GetAt(controlIndex);
			ret.x = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[0]);
			ret.y = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[1]);
			break;
		}
		break;
	case fbxsdk::FbxLayerElement::eByPolygonVertex:
		switch (refMode)
		{
		case fbxsdk::FbxLayerElement::eDirect:
			ret.x = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[0]);
			ret.y = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[1]);
			break;
		case fbxsdk::FbxLayerElement::eIndexToDirect:
			auto itd = uv->GetIndexArray().GetAt(index);
			ret.x = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[0]);
			ret.y = static_cast<float>(uv->GetDirectArray().GetAt(itd).mData[1]);
			break;
		}
		break;
	}

	return ret;
}

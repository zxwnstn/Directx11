#include "pch.h"

#include <fbxsdk.h>
#include "FbxLoader.h"
#include "FileCommon.h"

#include "Model/3D/Skeleton.h"
#include "Model/3D/SkeletalAnimation.h"

#include "Serialization.h"

namespace Engine {

	static FbxManager* s_FbxManager = nullptr;

	FBXLoader::FBXLoader()
	{
	}

	FBXLoader::~FBXLoader()
	{
		if (m_SkeletonName.empty()) return;
		SkeletonArchive::Get(m_SkeletonName)->ControlPoints.clear();
	}

	bool FBXLoader::Init(const std::string & skeletonName)
	{
		if (!s_FbxManager)
		{
			s_FbxManager = FbxManager::Create();

			FbxIOSettings* pIOsettings = FbxIOSettings::Create(s_FbxManager, IOSROOT);
			s_FbxManager->SetIOSettings(pIOsettings);
		}

		m_SkeletonName = skeletonName;
		return SkeletonArchive::Add(skeletonName);
	}

	void FBXLoader::Extract(const std::string & path, const std::filesystem::path &file)
	{
		//Initiate Scene
		FbxImporter* importer = FbxImporter::Create(s_FbxManager, "");
		FbxScene* scene = FbxScene::Create(s_FbxManager, "");

		m_FileName = file.stem().string();
		auto fullpath = path + file.filename().string();

		importer->Initialize(fullpath.c_str(), -1, s_FbxManager->GetIOSettings());
		importer->Import(scene);
		importer->Destroy();

		FbxNode* root = scene->GetRootNode();
		if (!isLoaded)
		{
			FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem::MayaYUp.ConvertScene(scene);
			FbxGeometryConverter geometryConverter(s_FbxManager);
			geometryConverter.Triangulate(scene, true);
		}
		SkeletalAnimationArchive::Add(m_SkeletonName, m_FileName);

		//Extract
		auto nodeCount = root->GetChildCount();
		if (isLoaded)
		{
			for (int i = 0; i < nodeCount; ++i)
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
			for (int i = 0; i < nodeCount; ++i)
			{
				FbxNode* node = root->GetChild(i);
				FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();

				switch (nodeType)
				{
				case fbxsdk::FbxNodeAttribute::eSkeleton:
					getJoints(root);
					break;
				case fbxsdk::FbxNodeAttribute::eMesh:
					getControlPoint(node);
					getAnimation(node);
					getVertices(node);
					break;
				}
			}
			isLoaded = true;
		}
	}

	void FBXLoader::getControlPoint(FbxNode* node)
	{
		if (isExistCache(Type::ControlPoints))
		{
			ImportCache(Type::ControlPoints);
			return;
		}

		FbxMesh* mesh = node->GetMesh();
		uint32_t count = mesh->GetControlPointsCount();
		auto& ControlPoints = SkeletonArchive::Get(m_SkeletonName)->ControlPoints;

		ControlPoints.resize(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			for (uint32_t j = 0; j < 3; ++j)
			{
				ControlPoints[i].Position.m[j] = static_cast<float>(mesh->GetControlPointAt(i).mData[j]);
			}
		}
	}

	namespace Detail {

		struct Type2
		{
			using ReturnType = vec2;
			static const int count = 2;
		};
		struct Type3
		{
			using ReturnType = vec3;
			static const int count = 3;
		};
	}

	template<typename Type, typename Elem>
	decltype(auto) getElement(Elem elem, int index, int controlIndex)
	{
		using Ret = Type::ReturnType;

		Ret ret;
		if (elem == nullptr) return ret;

		auto mapMode = elem->GetMappingMode();
		auto refMode = elem->GetReferenceMode();

		int myIndex;
		switch (mapMode)
		{
		case fbxsdk::FbxLayerElement::eByControlPoint:
			switch (refMode)
			{
			case fbxsdk::FbxLayerElement::eDirect: myIndex = controlIndex; break;
			case fbxsdk::FbxLayerElement::eIndexToDirect: myIndex = elem->GetIndexArray().GetAt(controlIndex); break;
			}
			break;
		case fbxsdk::FbxLayerElement::eByPolygonVertex:
			switch (refMode)
			{
			case fbxsdk::FbxLayerElement::eDirect: myIndex = index; break;
			case fbxsdk::FbxLayerElement::eIndexToDirect: myIndex = elem->GetIndexArray().GetAt(index); break;
			}
			break;
		}
		for (int i = 0; i < Type::count; ++i)
			ret.m[i] = static_cast<float>(elem->GetDirectArray().GetAt(myIndex).mData[i]);

		return ret;
	}
	

	void FBXLoader::getVertices(FbxNode* node)
	{
		FbxMesh* mesh = node->GetMesh();
		int count = mesh->GetPolygonCount();

		auto skeleton = SkeletonArchive::Get(m_SkeletonName);
		auto& ControlPoints = skeleton->ControlPoints;
		auto& Vertices = skeleton->Vertices;
		auto& Indices = skeleton->Indices;

		Indices.resize(count * 3);
		for (int i = 0; i < count; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				Indices[i * 3 + j] = i * 3 + j;
			}
		}

		if (isExistCache(Type::Vertices))
		{
			ImportCache(Type::Vertices);
			return;
		}

		Vertices.resize(count * 3);
		auto* uv = mesh->GetElementUV();
		auto* normal = mesh->GetElementNormal();
		auto* binormal = mesh->GetElementBinormal();
		auto* tangent = mesh->GetElementTangent();
		uint32_t index = 0;

		for (int i = 0; i < count; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				int controlIndex = mesh->GetPolygonVertex(i, j);
				Vertices[index].Position = ControlPoints[controlIndex].Position;
				Vertices[index].UV = getElement<Detail::Type2>(uv, index, controlIndex);
				Vertices[index].Normal = getElement<Detail::Type3>(normal, index, controlIndex);
				Vertices[index].BiNormal = getElement<Detail::Type3>(binormal, index, controlIndex);
				Vertices[index].Tangent = getElement<Detail::Type3>(tangent, index, controlIndex);

				for (int k = 0; k < 4; ++k)
				{
					Vertices[index].BoneIndex.m[k] = ControlPoints[controlIndex].BoneIndex.m[k];
					Vertices[index].BoneWeight.m[k] = ControlPoints[controlIndex].BoneWeight.m[k];
				}
				index++;
			}
		}
		ExportCache(Type::Vertices);
	}

	

	void FBXLoader::getJoints(FbxNode* node)
	{
		if (isExistCache(Type::Joints))
		{
			ImportCache(Type::Joints);
			return;
		}

		auto& joints = SkeletonArchive::Get(m_SkeletonName)->Joints;
		for (int child = 0; child < node->GetChildCount(); ++child)
		{
			FbxNode* cur = node->GetChild(child);
			getJoints(cur, 0, -1, joints);
		}
	}

	void FBXLoader::getJoints(FbxNode * node, int index, int parent, std::vector<Joint>& joints)
	{
		FbxNodeAttribute::EType type = node->GetNodeAttribute()->GetAttributeType();
		if (type == FbxNodeAttribute::EType::eSkeleton)
		{
			Joint joint;
			joint.Parent = parent;
			joint.Name = std::string(node->GetName());
			joints.push_back(joint);
		}
		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			getJoints(node->GetChild(i), (int)joints.size(), index, joints);
		}
	}

	void FBXLoader::getMaterial(FbxNode * node)
	{
	}

	void FBXLoader::getAnimation(FbxNode* node)
	{
		if (isExistCache(Type::Animation))
		{
			ImportCache(Type::Animation);
			return;
		}

		FbxMesh* mesh = node->GetMesh();
		unsigned int numOfDeformers = mesh->GetDeformerCount();
		FbxAMatrix geometryTransform;
		geometryTransform.SetIdentity();

		auto skeleton = SkeletonArchive::Get(m_SkeletonName);
		auto& JointAnimations = SkeletalAnimationArchive::GetAnimation(m_SkeletonName, m_FileName)->JointAnimations;
		auto& Joints = skeleton->Joints;
		auto& ControlPoints = skeleton->ControlPoints;

		//Extract
		for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
		{
			FbxSkin* skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
			if (!skin) continue;

			unsigned int numOfClusters = skin->GetClusterCount();
			JointAnimations.resize(numOfClusters);

			//for each Cluster (that contains link)
			for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);

				//0. Get JointIdex
				std::string jointName = cluster->GetLink()->GetName();
				unsigned int jointIndex;
				for (jointIndex = 0; jointIndex < Joints.size(); ++jointIndex)
					if (jointName == Joints[jointIndex].Name)
						break;

				if (!isLoaded)
				{
					//1. Calc OffsetMat
					{
						FbxAMatrix transformMatrix;
						FbxAMatrix transformLinkMatrix;
						FbxAMatrix offsetMat;
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
						Joints[jointIndex].Offset = DirectX::XMLoadFloat4x4(&offset);
					}

					//Get Joint Weight
					{
						unsigned int indices = cluster->GetControlPointIndicesCount();
						auto controlPointIndices = cluster->GetControlPointIndices();
						for (unsigned int i = 0; i < indices; ++i)
						{
							ControlPoints[controlPointIndices[i]].push((float)cluster->GetControlPointWeights()[i], jointIndex);
						}
					}
				}

				//Anim
				//Todo : Supply multiple Animation of one fbx file
				{
					FbxTime::EMode frameRate = FbxTime::EMode::eFrames30;
					uint32_t timePoint = 0;
					float lastStart = 0.0f;

					while (true)
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

						if (!JointAnimations[clusterIndex].KeyFrames.empty())
						{
							if (keyFrame == JointAnimations[clusterIndex].KeyFrames.back())
								break;
						}

						lastStart = keyFrame.Start;
						++timePoint;
						JointAnimations[clusterIndex].KeyFrames.push_back(keyFrame);
					}
					JointAnimations[clusterIndex].Duration = lastStart;
				}
			}
		}
		if (!isLoaded)
		{
			ExportCache(Type::Joints);
			ExportCache(Type::ControlPoints);
		}
		ExportCache(Type::Animation);
	}

	bool FBXLoader::isExistCache(Type type)
	{
		return File::isExistFile(GetCachePath(type));
	}

	std::string FBXLoader::GetCachePath(Type type)
	{
		switch (type)
		{
		case FBXLoader::Type::Vertices: return File::GetCommonPath(File::FBXCache) + m_SkeletonName + "/" + "Vertices";
		case FBXLoader::Type::ControlPoints: return File::GetCommonPath(File::FBXCache) + m_SkeletonName + "/" + "ControlPoints";
		case FBXLoader::Type::Joints: return File::GetCommonPath(File::FBXCache) + m_SkeletonName + "/" + "Joints";
		case FBXLoader::Type::Material: return File::GetCommonPath(File::FBXCache) + m_SkeletonName + "/" + "Material";
		case FBXLoader::Type::Animation:return File::GetCommonPath(File::FBXCache) + m_SkeletonName + "/" + m_FileName;
		}
		return "";
	}

	void FBXLoader::ExportCache(Type type)
	{
		auto path = GetCachePath(type);
		auto skeleton = SkeletonArchive::Get(m_SkeletonName);

		switch (type)
		{
		case FBXLoader::Type::Vertices:
		{
			auto& Vertices = skeleton->Vertices;
			Serializer::Write(path, Vertices);
		}
		break;
		case FBXLoader::Type::ControlPoints:
		{
			auto& ControlPoints = skeleton->ControlPoints;
			Serializer::Write(path, ControlPoints);
		}
		break;
		case FBXLoader::Type::Joints:
		{
			auto& Joints = skeleton->Joints;
			Serializer::Write(path, Joints);
		}
		break;
		case FBXLoader::Type::Material:
			break;
		case FBXLoader::Type::Animation:
		{
			auto& Animation = SkeletalAnimationArchive::GetAnimation(m_SkeletonName, m_FileName)->JointAnimations;
			Serializer::Write(path, Animation);
		}
		break;
		}
	}

	void FBXLoader::ImportCache(Type type)
	{
		auto path = GetCachePath(type);
		auto skeleton = SkeletonArchive::Get(m_SkeletonName);

		switch (type)
		{
		case FBXLoader::Type::Vertices:
		{
			auto& Vertices = skeleton->Vertices;
			Serializer::Read(path, Vertices);
		}
		break;
		case FBXLoader::Type::ControlPoints:
		{
			auto& ControlPoints = skeleton->ControlPoints;
			Serializer::Read(path, ControlPoints);
		}
		break;
		case FBXLoader::Type::Joints:
		{
			auto& Joints = skeleton->Joints;
			Serializer::Read(path, Joints);
		}
		break;
		case FBXLoader::Type::Material:
			break;
		case FBXLoader::Type::Animation:
		{
			auto& Animation = SkeletalAnimationArchive::GetAnimation(m_SkeletonName, m_FileName)->JointAnimations;
			Serializer::Read(path, Animation);
		}
		break;
		}
	}

}

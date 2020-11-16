#include "pch.h"

#include <fbxsdk.h>
#include "FbxLoader.h"
#include "FileCommon.h"

#include "Model/3D/Skeleton.h"
#include "Model/3D/SkeletalAnimation.h"
#include "Common/Material.h"
#include "Renderer/Texture.h"

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
		MaterialArchive::Add(skeletonName);
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
					getMaterial(node);
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

	namespace detail {

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
	
	std::pair<vec3, vec3> CalculateBinomal(vec3 p1, vec3 p2, vec3 p3, vec2 p1uv, vec2 p2uv, vec2 p3uv)
	{
		DirectX::XMFLOAT3 vertex1(p1.m[0], p1.m[1], p1.m[2]);
		DirectX::XMFLOAT3 vertex2(p2.m[0], p2.m[1], p2.m[2]);
		DirectX::XMFLOAT3 vertex3(p3.m[0], p3.m[1], p3.m[2]);
		
		DirectX::XMFLOAT3 binormal;
		DirectX::XMFLOAT3 tangent;
		vec3 retbi;
		vec3 retan;

		float vector1[3], vector2[3];
		float tuVector[2], tvVector[2];
		float den;
		float length;

		// Calculate the two vectors for this face.
		vector1[0] = vertex2.x - vertex1.x;
		vector1[1] = vertex2.y - vertex1.y;
		vector1[2] = vertex2.z - vertex1.z;

		vector2[0] = vertex3.x - vertex1.x;
		vector2[1] = vertex3.y - vertex1.y;
		vector2[2] = vertex3.z - vertex1.z;

		// Calculate the tu and tv texture space vectors.
		tuVector[0] = p2uv.m[0] - p1uv.m[0];
		tvVector[0] = p2uv.m[1] - p1uv.m[1];

		tuVector[1] = p3uv.m[0] - p1uv.m[0];
		tvVector[1] = p3uv.m[1] - p1uv.m[1];

		// Calculate the denominator of the tangent/binormal equation.
		den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

		// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
		tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
		tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
		tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

		binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
		binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
		binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

		// Calculate the length of this normal.
		length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

		// Normalize the normal and then store it
		tangent.x = tangent.x / length;
		tangent.y = tangent.y / length;
		tangent.z = tangent.z / length;

		// Calculate the length of this normal.
		length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

		// Normalize the normal and then store it
		binormal.x = binormal.x / length;
		binormal.y = binormal.y / length;
		binormal.z = binormal.z / length;

		retbi.m[0] = binormal.x;
		retbi.m[1] = binormal.y;
		retbi.m[2] = binormal.z;
		retan.m[0] = tangent.x;
		retan.m[1] = tangent.y;
		retan.m[2] = tangent.z;

		return { retbi , retan };
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
				Vertices[index].UV = getElement<detail::Type2>(uv, index, controlIndex);
				Vertices[index].Normal = getElement<detail::Type3>(normal, index, controlIndex);
				//Vertices[index].BiNormal = getElement<detail::Type3>(binormal, index, controlIndex);
				//Vertices[index].Tangent = getElement<detail::Type3>(tangent, index, controlIndex);
				for (int k = 0; k < 4; ++k)
				{
					Vertices[index].BoneIndex.m[k] = ControlPoints[controlIndex].BoneIndex.m[k];
					Vertices[index].BoneWeight.m[k] = ControlPoints[controlIndex].BoneWeight.m[k];
				}
				index++;
			}
			auto[binormal, tangent] = CalculateBinomal(Vertices[index - 3].Position, Vertices[index - 2].Position, Vertices[index - 1].Position,
				Vertices[index - 3].UV, Vertices[index - 2].UV, Vertices[index - 1].UV);

			Vertices[index - 1].Tangent = tangent;
			Vertices[index - 2].Tangent = tangent;
			Vertices[index - 3].Tangent = tangent;

			Vertices[index - 1].BiNormal = binormal;
			Vertices[index - 2].BiNormal = binormal;
			Vertices[index - 3].BiNormal = binormal;
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

	void getMaterialTexture(FbxSurfaceMaterial* pMaterial, const std::string& skeltonName)
	{
		unsigned int textureIndex = 0;
		FbxProperty property;

		auto mat = MaterialArchive::Get(skeltonName);
		FBXSDK_FOR_EACH_TEXTURE(textureIndex)
		{
			property = pMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
			if (property.IsValid())
			{
				unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
				for (unsigned int i = 0; i < textureCount; ++i)
				{
					FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);
					if (layeredTexture)
					{
						throw std::exception("Layered Texture is currently unsupported\n");
					}
					else
					{
						FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
						if (texture)
						{
							std::string textureType = property.GetNameAsCStr();
							FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);
							if (fileTexture)
							{
								auto path = fileTexture->GetFileName();
								if (textureType == "DiffuseColor")
								{
									auto textureName = skeltonName + "_diffuse";
									TextureArchive::Add(path, textureName, Texture::eDiffuse, 0);
									mat->MaterialTextures.emplace_back(textureName, Texture::eDiffuse);
								}
								if (textureType == "NormalMap")
								{
									auto textureName = skeltonName + "_normal";
									TextureArchive::Add(path, textureName, Texture::eNormal, 1);
									mat->MaterialTextures.emplace_back(textureName, Texture::eNormal);
								}
								if (textureType == "SpecularColor")
								{
									auto textureName = skeltonName + "_specular";
									TextureArchive::Add(path, textureName, Texture::eSpecular, 2);
									mat->MaterialTextures.emplace_back(textureName, Texture::eSpecular);
								}
							}
						}
					}
				}
			}
		}
	}

	void getMaterialAttribute(FbxSurfaceMaterial* pMaterial, const std::string& skeltonName)
	{
		FbxDouble3 double3;
		FbxDouble double1;
		auto mat = MaterialArchive::Get(skeltonName);
		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Amibent Color
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Ambient;
			mat->Ambient.x = static_cast<float>(double3.mData[0]);
			mat->Ambient.y = static_cast<float>(double3.mData[1]);
			mat->Ambient.z = static_cast<float>(double3.mData[2]);

			// Diffuse Color
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Diffuse;
			mat->Diffuse.x = static_cast<float>(double3.mData[0]);
			mat->Diffuse.y = static_cast<float>(double3.mData[1]);
			mat->Diffuse.z = static_cast<float>(double3.mData[2]);
			mat->Diffuse.w = static_cast<float>(double3.mData[3]);

			// Roughness 
			double1 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Shininess;
			mat->Roughness = float(1.0 - double1);

			// Reflection
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Reflection;
			mat->Fresnel.x = static_cast<float>(double3.mData[0]);
			mat->Fresnel.y = static_cast<float>(double3.mData[1]);
			mat->Fresnel.z = static_cast<float>(double3.mData[2]);

			// Specular Color
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Specular;
			mat->Specular.x = static_cast<float>(double3.mData[0]);
			mat->Specular.y = static_cast<float>(double3.mData[1]);
			mat->Specular.z = static_cast<float>(double3.mData[2]);

			// Emissive Color
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Emissive;
			mat->Emissive.x = static_cast<float>(double3.mData[0]);
			mat->Emissive.y = static_cast<float>(double3.mData[1]);
			mat->Emissive.z = static_cast<float>(double3.mData[2]);
		}
		else if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			// Amibent Color
			double3 = reinterpret_cast<FbxSurfaceLambert *>(pMaterial)->Ambient;
			mat->Ambient.x = static_cast<float>(double3.mData[0]);
			mat->Ambient.y = static_cast<float>(double3.mData[1]);
			mat->Ambient.z = static_cast<float>(double3.mData[2]);

			// Diffuse Color
			double3 = reinterpret_cast<FbxSurfaceLambert *>(pMaterial)->Diffuse;
			mat->Diffuse.x = static_cast<float>(double3.mData[0]);
			mat->Diffuse.y = static_cast<float>(double3.mData[1]);
			mat->Diffuse.z = static_cast<float>(double3.mData[2]);

			// Emissive Color
			double3 = reinterpret_cast<FbxSurfaceLambert *>(pMaterial)->Emissive;
			mat->Emissive.x = static_cast<float>(double3.mData[0]);
			mat->Emissive.y = static_cast<float>(double3.mData[1]);
			mat->Emissive.z = static_cast<float>(double3.mData[2]);
		}
	}
	void FBXLoader::getMaterial(FbxNode * node)
	{
		if (isExistCache(Type::Material))
		{
			ImportCache(Type::Material);
			return;
		}

		int count = node->GetMaterialCount();
		for (int i = 0; i < count; ++i)
		{
			
			FbxSurfaceMaterial* SurfaceMaterial = node->GetMaterial(i);
			getMaterialAttribute(SurfaceMaterial, m_SkeletonName);
			getMaterialTexture(SurfaceMaterial, m_SkeletonName);
		}
		ExportCache(Type::Material);
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
		{
			auto& Material = MaterialArchive::Get(m_SkeletonName);
			Serializer::Write(path, *Material);
		}
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
		{
			auto Material = MaterialArchive::Get(m_SkeletonName);
			Serializer::Read(path, *Material);
		}
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

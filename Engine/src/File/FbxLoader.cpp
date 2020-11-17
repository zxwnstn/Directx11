#include "pch.h"

#include <fbxsdk.h>
#include "FbxLoader.h"
#include "FileCommon.h"

#include "Model/3D/Skeleton.h"
#include "Model/3D/SkeletalAnimation.h"
#include "Common/Material.h"
#include "Renderer/Texture.h"

#include "Common/Timestep.h"
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
		MaterialArchive::AddSet(m_SkeletonName);

		//Create Chache folder
		auto dir = GetCachePath(Type::None);
		if (!File::isExistDirectroy(dir))
			File::CreateDir(dir);

		return SkeletonArchive::Add(skeletonName);
	}

	void FBXLoader::Extract(const std::string & path, const std::filesystem::path &file)
	{
		//Initiate Scene
		FbxImporter* importer = FbxImporter::Create(s_FbxManager, "");
		FbxScene* scene = FbxScene::Create(s_FbxManager, "");

		m_FileName = file.stem().string();
		auto fullpath = path + file.filename().string();

		std::cout << "\nFBXLoad::Extract with \"" << m_FileName << "\"\n";
		importer->Initialize(fullpath.c_str(), -1, s_FbxManager->GetIOSettings());
		importer->Import(scene);
		importer->Destroy();

		FbxNode* root = scene->GetRootNode();
		if (!loadedMesh)
		{
			FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem::MayaYUp.ConvertScene(scene);
			FbxGeometryConverter geometryConverter(s_FbxManager);

			Timestep::SetTimePoint();
			std::cout << "Process Triangulate... ";
			geometryConverter.Triangulate(scene, true);
			std::cout << "Completed : " << Timestep::Elapse() << "sec\n";
		}
		SkeletalAnimationArchive::Add(m_SkeletonName, m_FileName);

		//Extract
		auto nodeCount = root->GetChildCount();
		if (loadedMesh)
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
			std::cout << "***Start Geometry proess***\n";
#pragma region 1. Construct Skeleton
			std::cout << "Construct Skeleton Hierachy!... ";
			if (isExistCache(Type::Joints))
			{
				loadedSkeleton = true;
				ImportCache(Type::Joints);
				std::cout << "Completed by cache!\n";
			}		
			bool isConstructed = false;
			if (!loadedSkeleton)
			{
				for (int i = 0; i < nodeCount; ++i)
				{
					FbxNode* node = root->GetChild(i);
					FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();
					if (nodeType == fbxsdk::FbxNodeAttribute::eSkeleton)
					{
						isConstructed = true;
						getJoints(root);
					}
				}
				if (isConstructed)
				{
					std::cout << "Completed!\n";
				}
				else
				{
					std::cout << "Therer is no Skeleton!\n";
					__debugbreak();
				}
			}
			
#pragma endregion

#pragma region 2. Extract Mesh
			Timestep::Update();
			std::cout << "Extract mesh data!...\n";
			
			TryImport(Type::Material);
			TryImport(Type::Vertices);
			TryImport(Type::ControlPoints);

			if (!loadedControlPoint || !loadedVert || !loadedMaterial)
			{
				for (int i = 0; i < nodeCount; ++i)
				{
					FbxNode* node = root->GetChild(i);
					FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();

					if (nodeType == fbxsdk::FbxNodeAttribute::eMesh)
					{
						auto name = node->GetName();
						std::cout << "==== Processing " << name << "....\n";

						getMaterial(node);
						getControlPoint(node);
						getLinks(node);
						getVertices(node);
					}
				}
			}

			std::cout << "Processing Indices!...";
			{
				auto& indices = SkeletonArchive::Get(m_SkeletonName)->Indices;
				auto& vertices = SkeletonArchive::Get(m_SkeletonName)->Vertices;
				indices.resize(vertices.size());
				for (unsigned int i = 0; i < indices.size(); ++i)
				{
					indices[i] = i;
					vertices[i].check();
				}
				std::cout << "Completed!\n";
			}
			std::cout << "***Complete "<< m_SkeletonName << " geometry process! : " << Timestep::Elapse() << "sec ***\n";
			loadedMesh = true;
#pragma endregion

			TryExport(Type::Material);
			TryExport(Type::Vertices);
			TryExport(Type::ControlPoints);

			//getAnimation(node);
			//loadedAnimation = true;
		}
	}

	void FBXLoader::getControlPoint(FbxNode* node)
	{
		std::cout << "\tProcessing ControlPoint... ";
		if (loadedControlPoint)
		{
			std::cout << "Completed by cache...\n";
		}
		FbxMesh* mesh = node->GetMesh();
		uint32_t count = mesh->GetControlPointsCount();

		std::vector<ControlPoint> curControlPoints;
		curControlPoints.resize(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			for (uint32_t j = 0; j < 3; ++j)
			{
				curControlPoints[i].Position.m[j] = static_cast<float>(mesh->GetControlPointAt(i).mData[j]) * 0.01f;
			}
		}

		auto& ControlPoints = SkeletonArchive::Get(m_SkeletonName)->ControlPoints;
		std::string name = node->GetName();
		ControlPoints[name] = curControlPoints;
		std::cout << "Completed!\n";
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

	std::pair<vec3, vec3> CalculateBinomal(const vec3& p1, const vec3& p2, const vec3& p3,
		const vec2& p1uv, const vec2& p2uv, const vec2& p3uv)
	{
		// Calculate the two vectors for this face.
		vec3 vector1, vector2;
		for (int i = 0; i < 3; ++i)
		{
			vector1.m[i] = p2.m[i] - p1.m[i];
			vector2.m[i] = p3.m[i] - p1.m[i];
		}

		vec2 tuVector, tvVector;
		// Calculate the tu and tv texture space vectors.
		tuVector.m[0] = p2uv.m[0] - p1uv.m[0];
		tvVector.m[0] = p2uv.m[1] - p1uv.m[1];

		tuVector.m[1] = p3uv.m[0] - p1uv.m[0];
		tvVector.m[1] = p3uv.m[1] - p1uv.m[1];

		// Calculate the denominator of the tangent/binormal equation.
		vec3 tangent, binormal;
		
		float den = 1.0f / (tuVector.m[0] * tvVector.m[1] - tuVector.m[1] * tvVector.m[0]);

		static int i = 0;
		if (isnan(den) || isinf(den))
		{
			std::cout << "tangent binormal 0 " << i++ << "\n";
			return { tangent, binormal };
		}

		// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
		tangent.m[0] = (tvVector.m[1] * vector1.m[0] - tvVector.m[0] * vector2.m[0]) * den;
		tangent.m[1] = (tvVector.m[1] * vector1.m[1] - tvVector.m[0] * vector2.m[1]) * den;
		tangent.m[2] = (tvVector.m[1] * vector1.m[2] - tvVector.m[0] * vector2.m[2]) * den;
		binormal.m[0] = (tuVector.m[0] * vector2.m[0] - tuVector.m[1] * vector1.m[0]) * den;
		binormal.m[1] = (tuVector.m[0] * vector2.m[1] - tuVector.m[1] * vector1.m[1]) * den;
		binormal.m[2] = (tuVector.m[0] * vector2.m[2] - tuVector.m[1] * vector1.m[2]) * den;

		//Normalize
		float length = sqrt((tangent.m[0] * tangent.m[0]) + (tangent.m[1] * tangent.m[1]) + (tangent.m[2] * tangent.m[2]));
		tangent.m[0] = tangent.m[0] / length;
		tangent.m[1] = tangent.m[1] / length;
		tangent.m[2] = tangent.m[2] / length;

		length = sqrt((binormal.m[0] * binormal.m[0]) + (binormal.m[1] * binormal.m[1]) + (binormal.m[2] * binormal.m[2]));
		binormal.m[0] = binormal.m[0] / length;
		binormal.m[1] = binormal.m[1] / length;
		binormal.m[2] = binormal.m[2] / length;

		for (int i = 0; i < 3; ++i)
		{
			if (isnan(binormal.m[i]) || isnan(tangent.m[i]))
			{
				__debugbreak();
			}
		}


		return { tangent, binormal };
	}

	void FBXLoader::getVertices(FbxNode* node)
	{
		std::cout << "\tProcessing Vertices... ";
		if (loadedVert)
		{
			std::cout << "Completed by cache...\n";
		}

		FbxMesh* mesh = node->GetMesh();
		std::string nodeName = node->GetName();
		int count = mesh->GetPolygonCount();

		auto skeleton = SkeletonArchive::Get(m_SkeletonName);
		
		auto& ControlPoints = skeleton->ControlPoints[nodeName];
		auto& Vertices = skeleton->Vertices;

		auto* uv = mesh->GetElementUV();
		auto* normal = mesh->GetElementNormal();
		auto* binormal = mesh->GetElementBinormal();
		auto* tangent = mesh->GetElementTangent();
		uint32_t index = 0;

		for (int i = 0; i < count; ++i)
		{
			Vertex vertex[3];
			for (int j = 0; j < 3; ++j)
			{
				
				int controlIndex = mesh->GetPolygonVertex(i, j);
				vertex[j].Position = ControlPoints[controlIndex].Position;
				vertex[j].UV = getElement<detail::Type2>(uv, index, controlIndex);
				vertex[j].Normal = getElement<detail::Type3>(normal, index, controlIndex);

				if (binormal && tangent)
				{
					vertex[j].BiNormal = getElement<detail::Type3>(binormal, index, controlIndex);
					vertex[j].Tangent = getElement<detail::Type3>(tangent, index, controlIndex);
				}
				for (int k = 0; k < 4; ++k)
				{
					vertex[j].BoneWeight.m[k] = ControlPoints[controlIndex].BoneWeight.m[k];
					vertex[j].BoneWeightr.m[k] = ControlPoints[controlIndex].BoneWeightr.m[k];
					vertex[j].BoneIndex.m[k] = ControlPoints[controlIndex].BoneIndex.m[k];
					vertex[j].BoneIndexr.m[k] = ControlPoints[controlIndex].BoneIndexr.m[k];
				}
				index++;
			}

			if (!binormal || !tangent)
			{
				auto[binorm, tan] = CalculateBinomal(vertex[0].Position, vertex[1].Position, vertex[2].Position,
					vertex[0].UV, vertex[1].UV, vertex[2].UV);

				for (int k = 0; k < 3; ++k)
				{
					vertex[k].Tangent = binorm;
					vertex[k].BiNormal = tan;
					vertex[k].check();
					Vertices.push_back(vertex[k]);
				}
			}
		}
		std::cout << "Complete!\n";
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
		ExportCache(Type::Joints);
	}

	void FBXLoader::getLinks(FbxNode * node)
	{
		std::cout << "\tProcessing Links... ";
		if (loadedVert)
		{
			std::cout << "Completed by cache...\n";
		}

		std::string nodeName = node->GetName();
		FbxMesh* mesh = node->GetMesh();
		FbxAMatrix geometryTransform;
		geometryTransform.SetIdentity();

		auto skeleton = SkeletonArchive::Get(m_SkeletonName);
		auto& Joints = skeleton->Joints;
		auto& ControlPoints = skeleton->ControlPoints[nodeName];

		//Extract
		for (int deformerIndex = 0; deformerIndex < mesh->GetDeformerCount(); ++deformerIndex)
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
				for (jointIndex = 0; jointIndex < Joints.size(); ++jointIndex)
					if (jointName == Joints[jointIndex].Name)
						break;


				//1. Get joint OffsetMat
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

				//2. Get joint Weight-Index
				unsigned int indices = cluster->GetControlPointIndicesCount();
				auto controlPointIndices = cluster->GetControlPointIndices();
				for (unsigned int i = 0; i < indices; ++i)
				{
					float weight = (float)cluster->GetControlPointWeights()[i];
					if (-100 > weight && weight < 100)
						__debugbreak();

					ControlPoints[controlPointIndices[i]].push(weight, jointIndex);
				}
			}
		}
		std::cout << "Completed!\n";
	}

	void getMaterialTexture(FbxSurfaceMaterial* pMaterial, const std::string& skeltonName)
	{
		unsigned int textureIndex = 0;
		FbxProperty property;

		auto& matTextures = MaterialArchive::GetSet(skeltonName)->MaterialTextures;
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
						std::cout << "Layered Texture is currently unsupported\n";
						return;
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
								MaterialTextureInfo info;
								info.Path = path;
								if (textureType == "DiffuseColor")
								{
									auto textureName = skeltonName + "_diffuse";
									TextureArchive::Add(path, textureName, Texture::eDiffuse, 0);
									info.Name = textureName;
									info.UsageType = Texture::eDiffuse;
									matTextures.emplace_back(info);
								}
								if (textureType == "NormalMap")
								{
									auto textureName = skeltonName + "_normal";
									TextureArchive::Add(path, textureName, Texture::eNormal, 1);
									info.Name = textureName;
									info.UsageType = Texture::eNormal;
									matTextures.emplace_back(info);
								}
								if (textureType == "SpecularColor")
								{
									auto textureName = skeltonName + "_specular";
									TextureArchive::Add(path, textureName, Texture::eSpecular, 2);
									info.Name = textureName;
									info.UsageType = Texture::eSpecular;
									matTextures.emplace_back(info);
								}
							}
						}
					}
				}
			}
		}
	}

	Material getMaterialAttribute(FbxSurfaceMaterial* pMaterial, const std::string& skeltonName)
	{
		FbxDouble3 double3;
		FbxDouble4 double4;
		FbxDouble double1;
		Material mat;

		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Amibent Color
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Ambient;
			mat.Ambient.x = static_cast<float>(double3.mData[0]);
			mat.Ambient.y = static_cast<float>(double3.mData[1]);
			mat.Ambient.z = static_cast<float>(double3.mData[2]);

			// Diffuse Color
			double4 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Diffuse;
			mat.Diffuse.x = static_cast<float>(double4.mData[0]);
			mat.Diffuse.y = static_cast<float>(double4.mData[1]);
			mat.Diffuse.z = static_cast<float>(double4.mData[2]);
			mat.Diffuse.w = static_cast<float>(double4.mData[3]);

			// Roughness 
			double1 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Shininess;
			mat.Shiness = static_cast<float>(double1);

			// Reflection
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Reflection;
			mat.Fresnel.x = static_cast<float>(double3.mData[0]);
			mat.Fresnel.y = static_cast<float>(double3.mData[1]);
			mat.Fresnel.z = static_cast<float>(double3.mData[2]);

			// Specular Color
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Specular;
			mat.Specular.x = static_cast<float>(double3.mData[0]);
			mat.Specular.y = static_cast<float>(double3.mData[1]);
			mat.Specular.z = static_cast<float>(double3.mData[2]);

			// Emissive Color
			double3 = reinterpret_cast<FbxSurfacePhong *>(pMaterial)->Emissive;
			mat.Emissive.x = static_cast<float>(double3.mData[0]);
			mat.Emissive.y = static_cast<float>(double3.mData[1]);
			mat.Emissive.z = static_cast<float>(double3.mData[2]);
		}
		else if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			// Amibent Color
			double3 = reinterpret_cast<FbxSurfaceLambert *>(pMaterial)->Ambient;
			mat.Ambient.x = static_cast<float>(double3.mData[0]);
			mat.Ambient.y = static_cast<float>(double3.mData[1]);
			mat.Ambient.z = static_cast<float>(double3.mData[2]);

			// Diffuse Color
			double3 = reinterpret_cast<FbxSurfaceLambert *>(pMaterial)->Diffuse;
			mat.Diffuse.x = static_cast<float>(double3.mData[0]);
			mat.Diffuse.y = static_cast<float>(double3.mData[1]);
			mat.Diffuse.z = static_cast<float>(double3.mData[2]);

			// Emissive Color
			double3 = reinterpret_cast<FbxSurfaceLambert *>(pMaterial)->Emissive;
			mat.Emissive.x = static_cast<float>(double3.mData[0]);
			mat.Emissive.y = static_cast<float>(double3.mData[1]);
			mat.Emissive.z = static_cast<float>(double3.mData[2]);
		}
		return mat;
	}

	void FBXLoader::getMaterial(FbxNode * node)
	{
		std::cout << "\tProcessing Mertarial... ";
		if (loadedMaterial)
		{
			std::cout << "Completed by cache...\n";
		}
		int count = node->GetMaterialCount();
		std::string nodeName = node->GetName();
		if (!count)
		{
			std::cout << "Theres is no " << nodeName << "Material!\n";
			return;
		}

		FbxSurfaceMaterial* SurfaceMaterial = node->GetMaterial(0);
		auto materialSets = MaterialArchive::GetSet(m_SkeletonName);
		materialSets->materials[nodeName] = getMaterialAttribute(SurfaceMaterial, m_SkeletonName);
		if (materialSets->MaterialTextures.empty())
		{
			getMaterialTexture(SurfaceMaterial, m_SkeletonName);
		}

		std::cout << "Complete!\n";
	}

	void FBXLoader::getAnimation(FbxNode* node)
	{
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
		return File::GetCommonPath(File::FBXCache) + m_SkeletonName;
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
			auto& joints = skeleton->Joints;
			Serializer::Write(path, joints);
		}
		break;
		case FBXLoader::Type::Material:
		{
			auto Material = MaterialArchive::GetSet(m_SkeletonName);
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
			auto& joints = skeleton->Joints;
			Serializer::Read(path, joints);
		}
		break;
		case FBXLoader::Type::Material:
		{
			auto Material = MaterialArchive::GetSet(m_SkeletonName);
			Serializer::Read(path, *Material);
			for (auto& texture : Material->MaterialTextures)
			{
				TextureArchive::Add(texture.Path, texture.Name, Texture::UsageType(texture.UsageType));
			}
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

	std::string ToString(FBXLoader::Type type)
	{
		switch (type)
		{
		case Engine::FBXLoader::Type::Vertices: return "Vertices";
		case Engine::FBXLoader::Type::ControlPoints: return "ControlPoints";
		case Engine::FBXLoader::Type::Joints: return "Joints";
		case Engine::FBXLoader::Type::Material: return "Material";
		case Engine::FBXLoader::Type::Animation: return "Animation";
		}
		return "";
	}

	void Engine::FBXLoader::TryImport(Type type)
	{
		if (isExistCache(Type::Material))
		{
			switch (type)
			{
			case Engine::FBXLoader::Type::Vertices: loadedVert = true;
			case Engine::FBXLoader::Type::ControlPoints: loadedControlPoint = true;
			case Engine::FBXLoader::Type::Joints: loadedLink = true;
			case Engine::FBXLoader::Type::Material: loadedMaterial = true;
			}
			Timestep::Update();
			std::cout << "Import " << m_SkeletonName << " " << ToString(type) << " cache!...";
			ImportCache(type);
			std::cout << "Completed " << Timestep::Elapse() << "sec\n";
		}
	}

	void Engine::FBXLoader::TryExport(Type type)
	{
		if (!isExistCache(type))
		{
			loadedMaterial = true;
			Timestep::Update();
			std::cout << "Export " << m_SkeletonName << " " << ToString(type) << " cache!...";
			ExportCache(type);
			std::cout << "Completed " << Timestep::Elapse() << "sec\n";
		}
	}

}

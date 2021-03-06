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
	void postProcessingMaterialTexture(const std::string& skeletonName, const std::unordered_map<int, std::vector<MaterialTextureInfo>>& textures);
	
	FBXLoader::FBXLoader()
	{
		if (!s_FbxManager)
		{
			s_FbxManager = FbxManager::Create();

			FbxIOSettings* pIOsettings = FbxIOSettings::Create(s_FbxManager, IOSROOT);
			pIOsettings->SetBoolProp(IMP_FBX_ANIMATION, true);
			s_FbxManager->SetIOSettings(pIOsettings);
			ASSERT(s_FbxManager, "FBXLoader::Create FbxManager failed");
		}
	}

	FBXLoader::~FBXLoader()
	{
		if (m_SkeletonName.empty()) return;
		SkeletonArchive::Get(m_SkeletonName)->ControlPoints.clear();
	}

	void printJoint(const std::string& skeletonName)
	{
	#ifdef DEBUG_FEATURE
		auto joints = SkeletonArchive::Get(skeletonName)->Joints;
		std::cout << "Print skelton hierachy!\n";
		int idx = 0;
		for (auto it = joints.begin(); it != joints.end(); ++it)
		{
			std::cout << idx++ << " : " << it->Name << "\n";
		}
	#endif
	}

	bool FBXLoader::Init(const std::string & skeletonName)
	{
		//Create Chache folder
		auto dir = GetCachePath(Type::None);
		File::TryCreateDir(dir);

		m_SkeletonName = skeletonName;
		MaterialArchive::AddSet(m_SkeletonName);
		MeshArchive::AddSkeletalMesh(m_SkeletonName);
		return SkeletonArchive::Add(m_SkeletonName);
	}

	void FBXLoader::Extract(const std::string & path, const std::filesystem::path &file)
	{
		ENABLE_ELAPSE

		FbxScene* scene;
		FbxNode* root;

		m_FileName = file.stem().string();
		SkeletalAnimationArchive::Add(m_SkeletonName, m_FileName);
		loadedAnimation = false;

		LOG_INFO("FBXLoader::Extract with {0}/{1}", m_SkeletonName, m_FileName) {

			FbxImporter* importer = FbxImporter::Create(s_FbxManager, "");
			scene = FbxScene::Create(s_FbxManager, "");
			root = scene->GetRootNode();

			ASSERT(importer, "FBXLoader::Create Impoter Failed");
			ASSERT(scene, "FBXLoader::Create Scene Failed");
			ASSERT(root, "FBXLoader::Get root Failed");

			auto fullpath = path + file.filename().string();
			importer->Initialize(fullpath.c_str(), -1, s_FbxManager->GetIOSettings());
			importer->Import(scene);
			importer->Destroy();

			TryImport(Type::Material);
			TryImport(Type::Vertices);
			if (loadedMaterial && loadedVert) //not nessessary control points and links
				loadedMesh = true;

			if (!loadedMesh)
			{
				FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
				FbxAxisSystem::MayaYUp.ConvertScene(scene);
				FbxGeometryConverter geometryConverter(s_FbxManager);

				LOG_MISC("FBXLoader::Process Trinangulate") {
					ENABLE_ELAPSE
					geometryConverter.Triangulate(scene, true);
					LOG_ELAPSE
				}
			}
		}

		LOG_MISC("FBXLoader::Construct Skeleton Hierachy") {

			TryImport(Type::Joints);
			bool isConstructed = false;
			if (!loadedJoints)
			{
				for (int i = 0; i < root->GetChildCount(); ++i)
				{
					FbxNode* node = root->GetChild(i);
					FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();
					if (nodeType == fbxsdk::FbxNodeAttribute::eSkeleton)
					{
						getJoints(root);
						isConstructed = true;
					}
				}
				ASSERT(isConstructed, "FBXLoader::Construct Skeleton Failed");
				DEBUG_CALL(printJoint, m_SkeletonName);
				loadedJoints = true;
			}
		}

		LOG_MISC("FBXLoader::Extract mesh data") {

			if (!loadedMesh) 
			{
				LOG_MISC("FBXLoader::Start Geometry processing") {

					int materialIndex = 0;
					for (int i = 0; i < root->GetChildCount(); ++i)
					{
						FbxNode* node = root->GetChild(i);
						std::string nodeName = node->GetName();
						FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();

						if (nodeType == fbxsdk::FbxNodeAttribute::eMesh)
						{
							LOG_MISC("FBXLoader::Target Node -------------- {0}", nodeName);
							getMaterial(node, materialIndex);
							getControlPoint(node, materialIndex);
							getLinks(node);
							getVertices(node);
							++materialIndex;
						}
					}
					loadedMesh = true;
					loadedVert = true;
					loadedLink = true;
					loadedControlPoint = true;
					loadedMaterial = true;
					loadedJoints = true;

					TryExport(Type::Joints);
					TryExport(Type::Material);
					TryExport(Type::Vertices);
					TryExport(Type::ControlPoints);
				}
			}

			LOG_MISC("FBXLoader::Contruct indices") {

				ENABLE_ELAPSE
				auto& mesh = MeshArchive::GetSkeletalMesh(m_SkeletonName);
				mesh->Indices = MeshArchive::GetSerialIndices();
				mesh->IndiceCount = (uint32_t)mesh->Vertices.size();
				LOG_ELAPSE
			}
		}

		LOG_MISC("FBXLoader::Extract Animation") {
			
			TryImport(Type::Animation);
			if (!loadedAnimation)
				getAnimation(root);
			
			TryExport(Type::Animation);
		}

		LOG_INFO("FBXLoader::Extract Sucessfuly done {0}/{1}", m_SkeletonName, m_FileName)
		LOG_ELAPSE;
	}

	void FBXLoader::getControlPoint(FbxNode* node, int materialIndex)
	{
		if (loadedControlPoint)
		{
			LOG_MISC("FBXLoader::ControlPoint are loaded by chache");
			return;
		}
		
		LOG_MISC("FBXLoader::Processing ControlPoint") {

			ENABLE_ELAPSE
			FbxMesh* mesh = node->GetMesh();
			uint32_t count = mesh->GetControlPointsCount();

			std::vector<ControlPoint> curControlPoints;
			curControlPoints.resize(count);
			for (uint32_t i = 0; i < count; ++i)
			{
				for (uint32_t j = 0; j < 3; ++j)
				{
					curControlPoints[i].Position.m[j] = static_cast<float>(mesh->GetControlPointAt(i).mData[j]);
				}
				curControlPoints[i].MaterialIndex = materialIndex;
			}
			auto& ControlPoints = SkeletonArchive::Get(m_SkeletonName)->ControlPoints;
			std::string name = node->GetName();
			ControlPoints[name] = curControlPoints;
			
			LOG_ELAPSE
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

	void FBXLoader::getVertices(FbxNode* node)
	{
		if (loadedVert)
		{
			LOG_MISC("FBXLoader::Vertices are loaded by chache");
			return;
		}

		LOG_MISC("FBXLoader::Processing Vertices") {

			ENABLE_ELAPSE

			FbxMesh* mesh = node->GetMesh();
			std::string nodeName = node->GetName();

			auto skeleton = SkeletonArchive::Get(m_SkeletonName);
			auto& Vertices = MeshArchive::GetSkeletalMesh(m_SkeletonName)->Vertices;
			auto& ControlPoints = skeleton->ControlPoints[nodeName];

			auto* uv = mesh->GetElementUV();
			auto* normal = mesh->GetElementNormal();
			auto* binormal = mesh->GetElementBinormal();
			auto* tangent = mesh->GetElementTangent();

			int count = mesh->GetPolygonCount();
			uint32_t index = 0;
			for (int i = 0; i < count; ++i)
			{
				SkeletalVertex vertex[3];
				for (int j = 0; j < 3; ++j)
				{

					int controlIndex = mesh->GetPolygonVertex(i, j);
					vertex[j].Position = ControlPoints[controlIndex].Position;
					vertex[j].MaterialIndex = ControlPoints[controlIndex].MaterialIndex;
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
					auto[binorm, tan] = Util::GetTangentAndBinomal(vertex[0].Position, vertex[1].Position, vertex[2].Position,
						vertex[0].UV, vertex[1].UV, vertex[2].UV);

					for (int k = 0; k < 3; ++k)
					{
						vertex[k].Tangent = binorm;
						vertex[k].BiNormal = tan;
						DEBUG_CALL(vertex[k].checkValid);
						Vertices.push_back(vertex[k]);
					}
				}
			}
			LOG_ELAPSE
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

	void FBXLoader::getJoints(FbxNode* node)
	{
		if (loadedJoints)
		{
			LOG_MISC("FBXLoader::Joints are loaded by chache");
			return;
		}

		LOG_MISC("FBXLoader::Processing Joints") {

			ENABLE_ELAPSE
			auto& joints = SkeletonArchive::Get(m_SkeletonName)->Joints;
			for (int child = 0; child < node->GetChildCount(); ++child)
			{
				FbxNode* cur = node->GetChild(child);
				getJoints(cur, 0, -1, joints);
			}
			LOG_ELAPSE
		}
	}

	void FBXLoader::getLinks(FbxNode * node)
	{
		if (loadedLink)
		{
			LOG_MISC("FBXLoader::Links are loaded by chache");
			return;
		}

		LOG_MISC("FBXLoader::Processing Link") {
			
			ENABLE_ELAPSE

			std::string nodeName = node->GetName();
			FbxMesh* mesh = node->GetMesh();
			FbxAMatrix geometryTransform;
			geometryTransform.SetIdentity();

			auto skeleton = SkeletonArchive::Get(m_SkeletonName);
			auto& Joints = skeleton->Joints;
			auto& ControlPoints = skeleton->ControlPoints[nodeName];

			//Deformer -> cluster -> link(joint)
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

						cluster->GetTransformMatrix(transformMatrix);
						cluster->GetTransformLinkMatrix(transformLinkMatrix);
						offsetMat = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;
						for (int i = 0; i < 4; ++i)
						{
							for (int j = 0; j < 4; ++j)
							{
								Joints[jointIndex].Offset.m[i][j] = (float)offsetMat[i][j];
							}
						}
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
			LOG_ELAPSE
		}
	}

	std::vector<MaterialTextureInfo> getMaterialTexture(FbxSurfaceMaterial* pMaterial, const std::string& skeltonName, Material& material)
	{
		unsigned int textureIndex = 0;
		FbxProperty property;

		std::vector<MaterialTextureInfo> materialTexture;
		bool findD = false;
		bool findN = false;
		bool findS = false;
		materialTexture.resize(3);

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
						LOG_WARN("FBXLoader::Layered Texture is currently unsupported");
						return materialTexture;
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
								std::string n(path);
								LOG_WARN("path {0}", path);
								if (textureType == "DiffuseColor" && !findD)
								{
									auto k = n.rfind('/');
									auto textureName = n.substr(k);
									info.Name = textureName;
									materialTexture[0] = info;
									findD = true;
									material.MapMode |= 1;
								}
								if (textureType == "NormalMap" && !findN)
								{
									auto k = n.rfind('/');
									auto textureName = n.substr(k);
									info.Name = textureName;
									materialTexture[1] = info;
									findN = true;
									material.MapMode |= 2;
								}
								if (textureType == "SpecularColor" && !findS)
								{
									auto k = n.rfind('/');
									auto textureName = n.substr(k);
									info.Name = textureName;
									materialTexture[2] = info;
									findS = true;
									material.MapMode |= 4;
								}
							}
						}
					}
				}
			}
		}
		
		return materialTexture;
	}

	Material getMaterialAttribute(FbxSurfaceMaterial* pMaterial, const std::string& skeltonName)
	{
		FbxDouble3 double3;
		FbxDouble4 double4;
		FbxDouble double1;
		Material mat;

		mat.Name = pMaterial->GetName();

		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			LOG_MISC("Material type is phong");

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

			// Shiness 
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
			LOG_MISC("Material type is lambert");

			// Amibent Color
			double4 = reinterpret_cast<FbxSurfaceLambert *>(pMaterial)->Ambient;
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

	void FBXLoader::getMaterial(FbxNode * node, int materialIndex)
	{
		if (loadedMaterial)
		{
			LOG_MISC("FBXLoader::Materials are loaded by chache");
			return;
		}

		LOG_MISC("Processing Mertarial") {

			ENABLE_ELAPSE

			int count = node->GetMaterialCount();
			std::string nodeName = node->GetName();
			auto materialSets = MaterialArchive::GetSet(m_SkeletonName);

			if (!count)
			{
				LOG_MISC("Theres is no material on {0}'s node ", nodeName);
				return;
			}

			for (int i = 0; i < count; ++i) 
			{
				FbxSurfaceMaterial* SurfaceMaterial = node->GetMaterial(i);
				materialSets->Materials[materialIndex] = getMaterialAttribute(SurfaceMaterial, m_SkeletonName);
				materialSets->MaterialTextures[materialIndex] = getMaterialTexture(SurfaceMaterial, m_SkeletonName, materialSets->Materials[materialIndex]);
			}			
			LOG_ELAPSE
		}
	}

	void FBXLoader::getAnimation(FbxNode* root)
	{
		//getAnim(root->GetScene());
		if (loadedAnimation)
		{
			LOG_MISC("FBXLoader::Animation loaded by chache");
			return;
		}

		LOG_MISC("FBXLoader::Processing Animation") {

			ENABLE_ELAPSE

			auto skeleton = SkeletonArchive::Get(m_SkeletonName);
			auto& JointAnimations = SkeletalAnimationArchive::GetAnimation(m_SkeletonName, m_FileName)->JointAnimations;
			auto& Joints = skeleton->Joints;
			JointAnimations.resize(Joints.size());

			for (int i = 0; i < root->GetChildCount(); ++i)
			{
				FbxNode* node = root->GetChild(i);
				FbxMesh* mesh = node->GetMesh();
				if (mesh == nullptr)
					continue;

				int count = mesh->GetDeformerCount();
				FbxAMatrix geometryTransform;
				geometryTransform.SetIdentity();

				//deformer -> cluster -> link(joint)
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

						if (!JointAnimations[jointIndex].KeyFrames.empty())
							continue;

						//1. Anim
						FbxTime::EMode frameRate = FbxTime::EMode::eFrames30;
						uint32_t timePoint = 0;
						float lastStart = 0.0f;

						while (true)
						{
							FbxTime curTime;
							KeyFrame keyFrame;

							curTime.SetFrame(timePoint, frameRate);
							keyFrame.Start = (float)curTime.GetSecondDouble();

							FbxAMatrix currentTransformOffset = root->EvaluateGlobalTransform(curTime) * geometryTransform;
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

							if (!JointAnimations[jointIndex].KeyFrames.empty())
							{
								if (keyFrame == JointAnimations[jointIndex].KeyFrames.back())
									break;
							}

							lastStart = keyFrame.Start;
							++timePoint;
							JointAnimations[jointIndex].KeyFrames.push_back(keyFrame);
						}
						JointAnimations[jointIndex].Duration = lastStart;
					}
				}
			}
			if (postProcessingAnimation(JointAnimations))
			{
				loadedAnimation = true;
			}
			else
			{
				LOG_WARN("{0}/{1} animtion load fail!", m_SkeletonName, m_FileName);
			}
			LOG_ELAPSE
		}
	}

	bool FBXLoader::postProcessingAnimation(std::vector<JointAnimation>& animation)
	{
		std::vector<Engine::KeyFrame> refKeyFrames;
		for (int i = 0; i < animation.size(); ++i)
		{
			if (!animation[i].KeyFrames.empty())
			{
				refKeyFrames = animation[i].KeyFrames;
				break;
			}
		}

		if (refKeyFrames.empty())
		{
			SkeletalAnimationArchive::Delete(m_SkeletonName, m_FileName);
			return false;
		}

		KeyFrame defualt;
		defualt.Scale.x = 1.0f;
		defualt.Scale.y = 1.0f;
		defualt.Scale.z = 1.0f;

		for (int i = 0; i < animation.size(); ++i)
		{
			if (animation[i].KeyFrames.empty())
			{
				for (int j = 0; j < refKeyFrames.size(); ++j)
				{
					defualt.Start = refKeyFrames[j].Start;
					animation[i].KeyFrames.push_back(defualt);
				}
			}
		}
		return true;
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
		File::TryCreateFile(path);
		auto skeleton = SkeletonArchive::Get(m_SkeletonName);
		auto mesh = MeshArchive::GetSkeletalMesh(m_SkeletonName);

		switch (type)
		{
		case FBXLoader::Type::Vertices:
		{
			auto& Vertices = mesh->Vertices;
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
			for (auto&[n, v] : Material->MaterialTextures)
			{
				for (auto& texture : v)
				{
					TextureArchive::Add(texture.Path, texture.Name);
				}
			}
		}
		break;
		case FBXLoader::Type::Animation:
		{
			auto& animation = SkeletalAnimationArchive::GetAnimation(m_SkeletonName, m_FileName)->JointAnimations;
			Serializer::Write(path, animation);
		}
		break;
		}
	}

	void FBXLoader::ImportCache(Type type)
	{
		auto path = GetCachePath(type);
		auto skeleton = SkeletonArchive::Get(m_SkeletonName);
		auto mesh = MeshArchive::GetSkeletalMesh(m_SkeletonName);

		switch (type)
		{
		case FBXLoader::Type::Vertices:
		{
			auto& Vertices = mesh->Vertices;
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

			for (auto&[n, v] : Material->MaterialTextures)
			{
				for (auto& texture : v)
				{
					TextureArchive::Add(texture.Path, texture.Name);
				}
			}

		}
		break;
		case FBXLoader::Type::Animation:
		{
			auto& animation = SkeletalAnimationArchive::GetAnimation(m_SkeletonName, m_FileName)->JointAnimations;
			Serializer::Read(path, animation);
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
		if (isExistCache(type))
		{
			switch (type)
			{
			case Engine::FBXLoader::Type::Vertices: loadedVert = true; loadedLink = true; break;
			case Engine::FBXLoader::Type::ControlPoints: loadedControlPoint = true; break;
			case Engine::FBXLoader::Type::Joints: loadedJoints = true;  loadedLink = true; break;
			case Engine::FBXLoader::Type::Material: loadedMaterial = true; break;
			case Engine::FBXLoader::Type::Animation: loadedAnimation = true; break;
			}
			ENABLE_ELAPSE
			LOG_MISC("Import {0} {1} cache!", m_SkeletonName, ToString(type));
			ImportCache(type);
			LOG_ELAPSE
		}
	}

	void Engine::FBXLoader::TryExport(Type type)
	{
		switch (type)
		{
		case Engine::FBXLoader::Type::Vertices: if (!loadedVert) return; break;
		case Engine::FBXLoader::Type::ControlPoints: if (!loadedControlPoint) return; break;
		case Engine::FBXLoader::Type::Joints: if (!loadedJoints || !loadedLink) return; break;
		case Engine::FBXLoader::Type::Material: if (!loadedMaterial) return; break;
		case Engine::FBXLoader::Type::Animation: if (!loadedAnimation) return; break;
		}

		if (!isExistCache(type))
		{
			ENABLE_ELAPSE
			LOG_MISC("Export {0} {1} cache!", m_SkeletonName, ToString(type));
			ExportCache(type);
			LOG_ELAPSE
		}
	}

}
#pragma once

#include "Util/MeshInfo.h"

namespace Engine {

	class FBXLoader
	{
	public:
		enum class Type
		{
			Vertices,
			ControlPoints,
			Joints,
			Material,
			Animation, 
			None
		};

	public:
		FBXLoader();
		~FBXLoader();

	public:
		bool Init(const std::string& skeletonName);
		void Extract(const std::string & path, const std::filesystem::path& file);

	private:
		void getControlPoint(class FbxNode* node, int materialIndex);
		void getMaterial(FbxNode* node, int materialIndex);
		void getVertices(FbxNode* node);
		void getJoints(FbxNode* node, int index, int parent, std::vector<struct Joint>& joints);
		void getJoints(FbxNode* node);
		void getLinks(FbxNode* node);

		bool postProcessingAnimation(std::vector<struct JointAnimation>& animation);
		void getAnimation(FbxNode* root);

		//this func is in experiment
		//void getAnimation(FbxNode* root, FbxNode* node, class FbxAnimLayer* animLayer);

	private:
		bool isExistCache(Type type);
		std::string GetCachePath(Type type);

		void ExportCache(Type type);
		void ImportCache(Type type);

		void TryImport(Type type);
		void TryExport(Type type);



	private:
		std::string m_SkeletonName;
		std::string m_FileName;

		bool loadedMesh			= false;

		bool loadedJoints		= false;
		bool loadedVert			= false;
		bool loadedControlPoint = false;
		bool loadedLink			= false;
		bool loadedMaterial		= false;
		bool loadedAnimation	= false;
	};

}

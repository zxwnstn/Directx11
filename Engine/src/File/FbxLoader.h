#pragma once

#include "Model/3D/Skeleton.h"

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
			Animation
		};

	public:
		FBXLoader();
		~FBXLoader();

	public:
		bool Init(const std::string& skeletonName);
		void Extract(const std::string & path, const std::filesystem::path& file);

	private:
		void getControlPoint(class FbxNode* node);
		void getVertices(FbxNode* node);
		void getJoints(FbxNode* node);
		void getJoints(FbxNode* node, int index, int parent, std::vector<Joint>& joints);
		void getAnimation(FbxNode* node);
		void getMaterial(FbxNode* node);

	private:
		bool isExistCache(Type type);
		std::string GetCachePath(Type type);

		void ExportCache(Type type);
		void ImportCache(Type type);

	private:
		std::string m_SkeletonName;
		std::string m_FileName;

		bool isLoaded = false;
	};

}

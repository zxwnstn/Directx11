#include "pch.h"

#include "Skeleton.h"
#include "File/FbxLoader.h"

namespace Engine {

	static std::unordered_map<std::string, std::shared_ptr<Skeleton>> s_Skeletons;

	bool SkeletonArchive::Add(const std::string& name)
	{
		auto find = s_Skeletons.find(name);
		if (find != s_Skeletons.end()) return false;

		auto skeleton = std::shared_ptr<Skeleton>(new Skeleton);
		skeleton->Name = name;

		s_Skeletons.emplace(name, skeleton);
		return true;
	}

	bool SkeletonArchive::Has(const std::string & name)
	{
		auto find = s_Skeletons.find(name);
		return find != s_Skeletons.end();
	}

	std::shared_ptr<Skeleton> SkeletonArchive::Get(const std::string & name)
	{
		if (!Has(name))
			return nullptr;

		return s_Skeletons[name];
	}

	void SkeletonArchive::Shutdown()
	{
		s_Skeletons.clear();
	}

}

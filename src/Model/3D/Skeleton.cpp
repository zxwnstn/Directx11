#include "pch.h"

#include "Skeleton.h"
#include "File/FbxLoader.h"

static std::unordered_map<std::string, std::weak_ptr<Skeleton>> s_Skeletons;

void SkeletonArchive::Add(const std::string& name)
{
	auto find = s_Skeletons.find(name);
	if (find != s_Skeletons.end()) return;

	s_Skeletons.emplace(name, std::weak_ptr<Skeleton>());
}

bool SkeletonArchive::Has(const std::string & name)
{
	auto find = s_Skeletons.find(name);
	return find != s_Skeletons.end();
}

std::shared_ptr<Skeleton> SkeletonArchive::Get(const std::string & name)
{
	if(!Has(name))
		return nullptr;

	if (s_Skeletons[name].expired())
	{
		//TODO : FBX load
	}

	auto ret = s_Skeletons[name].lock();
	return ret;
}


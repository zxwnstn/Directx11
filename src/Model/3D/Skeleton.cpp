#include "pch.h"

#include "Skeleton.h"
#include "File/FbxLoader.h"

static std::unordered_map<std::string, Skeleton*> s_Skeletons;

void SkeletonArchive::Add(Skeleton* skeleton)
{
	auto find = s_Skeletons.find(skeleton->Name);
	if (find != s_Skeletons.end()) return;

	s_Skeletons.emplace(skeleton->Name, skeleton);
}

bool SkeletonArchive::Has(const std::string & name)
{
	auto find = s_Skeletons.find(name);
	return find != s_Skeletons.end();
}

Skeleton* SkeletonArchive::Get(const std::string & name)
{
	if(!Has(name))
		return nullptr;

	return s_Skeletons[name];
}


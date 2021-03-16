#include "pch.h"

#include "Mesh.h"

namespace Engine {

	bool SkeletalVertex::checkValid()
	{
	#ifdef DEBUG_FEATURE
		for (int i = 0; i < 3; ++i)
		{
			if (!(-1000.0f <= Position.m[i] && Position.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= Normal.m[i] && Normal.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BiNormal.m[i] && BiNormal.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= Tangent.m[i] && Tangent.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			if (!(-1000.0f <= BoneWeight.m[i] && BoneWeight.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BoneIndex.m[i] && BoneIndex.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BoneWeightr.m[i] && BoneWeightr.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BoneIndexr.m[i] && BoneIndexr.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
		}
		return true;
	#endif
		return true;
	}

	void ControlPoint::push(float weight, uint32_t index)
	{
		if (i >= 4)
		{
			BoneWeightr.m[i % 4] = weight;
			BoneIndexr.m[i % 4] = index;
			++i;
			return;
		}
		BoneWeight.m[i] = weight;
		BoneIndex.m[i] = index;
		++i;
	}


	MeshArchive::SerialIndecies MeshArchive::s_SerialIndecies = nullptr;
	std::unordered_map<std::string, std::shared_ptr<StaticMesh>> MeshArchive::s_StaticMeshes;
	std::unordered_map<std::string, std::shared_ptr<SkeletalMesh>> MeshArchive::s_SkeletalMeshes;

	void MeshArchive::Init()
	{
		s_SerialIndecies = new uint32_t[MAXIMUM_INDICES];
		for (uint32_t i = 0; i < MAXIMUM_INDICES; ++i)
		{
			s_SerialIndecies[i] = i;
		}
	}

	void MeshArchive::ShutDown()
	{
		delete[] s_SerialIndecies;
		s_StaticMeshes.clear();
		s_SkeletalMeshes.clear();
	}

	std::shared_ptr<StaticMesh> MeshArchive::AddStaticMesh(const std::string & name)
	{
		if (HasStaticMesh(name))
		{
			LOG_WARN("{0} static mesh already exist", name);
			return nullptr;
		}

		s_StaticMeshes[name].reset(new StaticMesh);
		return s_StaticMeshes[name];
	}

	std::shared_ptr<SkeletalMesh> MeshArchive::AddSkeletalMesh(const std::string & name)
	{
		if (HasSkeletalMesh(name))
		{
			LOG_WARN("{0} skeletal mesh already exist", name);
			return nullptr;
		}

		s_SkeletalMeshes[name].reset(new SkeletalMesh);
		return s_SkeletalMeshes[name];
	}

	std::shared_ptr<StaticMesh> MeshArchive::GetStaticMesh(const std::string & name)
	{
		if (!HasStaticMesh(name))
		{
			LOG_WARN("{0} static mesh does not exist", name);
			return nullptr;
		}
		return s_StaticMeshes[name];
	}

	bool MeshArchive::HasStaticMesh(const std::string & name)
	{
		auto find = s_StaticMeshes.find(name);
		return find != s_StaticMeshes.end();
	}

	std::shared_ptr<SkeletalMesh> MeshArchive::GetSkeletalMesh(const std::string & name)
	{
		if (!HasSkeletalMesh(name))
		{
			LOG_WARN("{0} skeletal mesh does not exist", name);
			return nullptr;
		}
		return s_SkeletalMeshes[name];
	}

	std::vector<std::string> MeshArchive::GetStaticMeshList()
	{
		std::vector<std::string> ret;

		for (auto&[name, mesh] : s_StaticMeshes)
			ret.push_back(name);

		return ret;
	}

	std::vector<std::string> MeshArchive::GetSkeletalMeshList()
	{
		std::vector<std::string> ret;

		for (auto&[name, mesh] : s_SkeletalMeshes)
			ret.push_back(name);

		return ret;
	}

	bool MeshArchive::HasSkeletalMesh(const std::string & name)
	{
		auto find = s_SkeletalMeshes.find(name);
		return find != s_SkeletalMeshes.end();
	}

	uint32_t GetStride(MeshType type)
	{
		switch (type)
		{
		case Engine::MeshType::Skeletal: return sizeof(SkeletalVertex);
		case Engine::MeshType::Static: return sizeof(Vertex);
		case Engine::MeshType::Sqaure: return 20;
		}
		return 0;
	}

}


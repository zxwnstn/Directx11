#include "pch.h"

#include "Material.h"

namespace Engine {

	static std::unordered_map<std::string, std::shared_ptr<Material>> s_Materials;
	static std::unordered_map<std::string, std::shared_ptr<MaterialSet>> s_MaterialSets;

	std::shared_ptr<Material> Engine::MaterialArchive::Add(const std::string & name)
	{
		if (Has(name))
		{
			std::cout << "Aleady exist " << name << " material!";
			return nullptr;
		}

		std::shared_ptr<Material> material(new Material);
		s_Materials[name] = material;
		return s_Materials[name];
	}

	std::shared_ptr<MaterialSet> MaterialArchive::AddSet(const std::string & name)
	{
		if (HasSet(name))
		{
			std::cout << "Aleady exist " << name << " material sSet!";
			return nullptr;
		}

		std::shared_ptr<MaterialSet> materialset(new MaterialSet);
		s_MaterialSets[name] = materialset;
		return s_MaterialSets[name];
	}

	bool MaterialArchive::Has(const std::string & name)
	{
		auto find = s_Materials.find(name);
		return find != s_Materials.end();
	}

	bool MaterialArchive::HasSet(const std::string & name)
	{
		auto find = s_MaterialSets.find(name);
		return find != s_MaterialSets.end();
	}

	std::shared_ptr<Material> MaterialArchive::Get(const std::string & name)
	{
		if (!Has(name))
		{
			std::cout << "Doesn't exist " << name << " material!";
			return nullptr;
		}

		return s_Materials[name];
	}

	std::shared_ptr<MaterialSet> MaterialArchive::GetSet(const std::string & name)
	{
		if (!HasSet(name))
		{
			std::cout << "Doesn't exist " << name << " material set!";
			return nullptr;
		}

		return s_MaterialSets[name];
	}

}


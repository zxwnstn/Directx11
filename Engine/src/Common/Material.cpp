#include "pch.h"

#include "Material.h"
#include "Renderer/Texture.h"

namespace Engine {

	void MaterialSet::BindTextures(int slot)
	{
		std::vector<std::string> textures;
		for (int i = 0; i < MaterialTextures.size(); ++i)
		{
			auto& textureSet = MaterialTextures.find(i)->second;
			for (int j = 0; j < textureSet.size(); ++j)
			{
				textures.push_back(textureSet[j].Name);
			}
		}
		Texture::MultipleTextureBind(textures, slot);
	}

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

	std::shared_ptr<MaterialSet> MaterialArchive::GetSetCopy(const std::string & name)
	{
		if (!HasSet(name))
		{
			std::cout << "Doesn't exist " << name << " material set!";
			return nullptr;
		}
		std::shared_ptr<MaterialSet> ret;
		ret.reset(new MaterialSet(*s_MaterialSets[name]));
		return ret;
	}
	

}


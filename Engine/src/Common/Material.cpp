#include "pch.h"

#include "Material.h"

namespace Engine {

	static std::unordered_map<std::string, std::shared_ptr<Material>> s_Materials;

	void Engine::MaterialArchive::Add(const std::string & name)
	{
		if (Has(name))
		{
			std::cout << "Aleady exist " << name << " material!";
			return;
		}

		std::shared_ptr<Material> material(new Material);
		s_Materials[name] = material;
	}

	bool MaterialArchive::Has(const std::string & name)
	{
		auto find = s_Materials.find(name);
		return find != s_Materials.end();
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

	void Material::Bind() const
	{
	}

	void Material::SetMaterialTexture(Texture::UsageType type, bool enable)
	{
		for (auto& texture : MaterialTextures)
		{
			auto usageType = TextureArchive::Get(texture)->Type();
			if (usageType == type)
			{
				if (enable)
				{
					MMode |= type;
				}
				else
				{
					if (MMode & type)
					{
						MMode ^= type;
					}
				}
				break;
			}
		}
	}

}


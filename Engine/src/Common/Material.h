#pragma once

#include "Util/Math.h"

namespace Engine {

	struct MaterialTextureInfo
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Path;
			ar & Name;
		}
		std::string Path;
		std::string	Name;
	};

	struct Material
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Ambient.x; ar & Ambient.y; ar & Ambient.z;
			ar & Diffuse.x; ar & Diffuse.y; ar & Diffuse.z;
			ar & Specular.x; ar & Specular.y; ar & Specular.z;
			ar & Emissive.x; ar & Emissive.y; ar & Emissive.z;
			ar & Fresnel.x; ar & Fresnel.y; ar & Fresnel.z;
			ar & Shiness; ar & MapMode;
		}
		vec4 Ambient;
		vec4 Diffuse;
		vec4 Specular;
		vec4 Emissive;
		vec4 Fresnel;
		float Shiness;
		int MapMode = 0;
	};

	struct MaterialSet
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Materials;
			ar & MaterialTextures;
		}

		//MaterialIndex, Material
		std::unordered_map<int, Material> Materials;
		std::unordered_map<int, std::vector<MaterialTextureInfo>> MaterialTextures;

		void BindTextures(int slot);
	};

	class MaterialArchive
	{
	public:
		static std::shared_ptr<Material> Add(const std::string& name);
		static std::shared_ptr<MaterialSet> AddSet(const std::string& name);

		static bool Has(const std::string& name);
		static bool HasSet(const std::string& name);

		static std::shared_ptr<Material> Get(const std::string& name);
		static std::shared_ptr<MaterialSet> GetSet(const std::string& name);
	};
}

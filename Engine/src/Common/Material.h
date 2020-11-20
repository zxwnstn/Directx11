#pragma once

#include "Renderer/Texture.h"

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
			ar & Diffuse.x; ar & Diffuse.y; ar & Diffuse.z; ar & Diffuse.w;
			ar & Specular.x; ar & Specular.y; ar & Specular.z;
			ar & Emissive.x; ar & Emissive.y; ar & Emissive.z;
			ar & Fresnel.x; ar & Fresnel.y; ar & Fresnel.z;
			ar & Shiness; ar & MMode;
		}
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT4 Emissive;
		DirectX::XMFLOAT4 Fresnel;
		float Shiness;
		int MMode = 0;
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
	};

	class MaterialArchive
	{
	public:
		static void Add(const std::string& name);
		static void AddSet(const std::string& name);

		static bool Has(const std::string& name);
		static bool HasSet(const std::string& name);

		static std::shared_ptr<Material> Get(const std::string& name);
		static std::shared_ptr<MaterialSet> GetSet(const std::string& name);
	};
}

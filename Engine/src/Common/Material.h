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
			ar & UsageType;
		}
		std::string Path;
		std::string	Name;
		int UsageType;
	};

	struct Material
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Ambient.x; ar & Ambient.y; ar & Ambient.z;
			ar & Diffuse.x; ar & Diffuse.y; ar & Diffuse.z; ar & Diffuse.w;
			ar & Fresnel.x; ar & Fresnel.y; ar & Fresnel.z;
			ar & Specular.x; ar & Specular.y; ar & Specular.z;
			ar & Emissive.x; ar & Emissive.y; ar & Emissive.z;
			ar & Shiness;
		}
		DirectX::XMFLOAT3 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT3 Specular;
		DirectX::XMFLOAT3 Fresnel;
		DirectX::XMFLOAT3 Emissive;
		float Shiness;
	};

	struct MaterialSet
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & MMode;
			ar & materials;
			ar & MaterialTextures;
		}

		//for fbx model(if they has multiple materials)
		int MMode = 0;
		std::unordered_map<std::string, Material> materials;
		std::vector<MaterialTextureInfo> MaterialTextures;
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

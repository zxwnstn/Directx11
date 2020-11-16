#pragma once

#include "Renderer/Texture.h"

namespace Engine {

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
			ar & Roughness;
			ar & MaterialTextures;
		}
		void Bind() const;
		void SetMaterialTexture(Texture::UsageType type, bool enable = true);

		int MMode = 0;
		DirectX::XMFLOAT3 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT3 Specular;
		DirectX::XMFLOAT3 Fresnel;
		DirectX::XMFLOAT3 Emissive;
		float Roughness;

		std::vector<std::string> MaterialTextures;

	};

	class MaterialArchive
	{
	public:
		static void Add(const std::string& name);
		static bool Has(const std::string& name);
		static std::shared_ptr<Material> Get(const std::string& name);
	};
	
}

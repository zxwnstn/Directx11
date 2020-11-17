#pragma once

#include "Core/Base.h"

namespace Engine {

	class Texture
	{
	public:
		enum UsageType : int
		{
			eDiffuse = BIT(0),
			eNormal = BIT(1),
			eSpecular = BIT(2),
			eCustom = eSpecular + 1,
		};

	public:
		Texture(const std::string& path, UsageType type, int mySlot);

		void Bind(int slot) const;
		void Bind() const;

		static void MultipleBind(const std::vector<std::shared_ptr<Texture>>& textures);

		inline const UsageType Type() const { return type; }

	private:
		int32_t Width;
		int32_t Height;
		int32_t Channels;

		int MySlot;
		UsageType type = eCustom;

		ID3D11Texture2D* Buffer;
		ID3D11ShaderResourceView* View;

		friend class TextureArchive;
		friend class Renderer;
	};

	class TextureArchive
	{
	public:
		static void Add(const std::string& path, const std::string& name, Texture::UsageType type = Texture::eCustom, int slot = 0);
		static bool Has(const std::string& name);
		static std::shared_ptr<Texture> Get(const std::string& name);
		static void Shudown();
	};

}

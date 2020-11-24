#pragma once

#include "Core/Base.h"

namespace Engine {

	class Texture
	{
	private:
		Texture(uint32_t width, uint32_t height, int slot); // Render target Texture;
		Texture(const std::string& path, int mySlot);
		Texture(const std::vector<std::string>& paths, int mySlot);

	public:
		void Bind(int slot) const;
		void Bind() const;

		static void MultipleTextureBind(const std::vector<std::string>& textures, int slot = 0);

	private:
		int32_t Width;
		int32_t Height;
		int32_t Channels;

		uint32_t MySlot = 0;
		uint32_t TextureArrayMax;
		bool isTextureArray;

		ID3D11Texture2D* Buffer = nullptr;
		ID3D11RenderTargetView* RenderTargetView = nullptr;
		ID3D11ShaderResourceView* View = nullptr;

		friend class TextureArchive;
		friend class Renderer;
		friend class PipelineController;
	};

	class TextureArchive
	{
	public:
		static void Add(const std::string& name, uint32_t width, uint32_t height, int slot = 0); //for Render target texture
		static void Add(const std::string& path, const std::string& name, int slot = 0);
		static void Add(const std::vector<std::string>& paths, const std::string& name, int slot = 0);
		static bool Has(const std::string& name);
		static std::shared_ptr<Texture> Get(const std::string& name);
		static void Shudown();
	};

}

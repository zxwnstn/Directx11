#pragma once

#include "Core/Base.h"

namespace Engine {

	class Texture
	{
	private:
		class RTTInform
		{
		public:
			RTTInform(uint32_t width, uint32_t height, ID3D11Texture2D* buffer, bool isBackBuffer = false);

		public:
			void Resize(uint32_t width, uint32_t height, ID3D11Texture2D* buffer);

		private:
			ID3D11Texture2D* m_DepthStecilBuffer = nullptr;
			ID3D11RenderTargetView* m_RenderTargetView = nullptr;
			ID3D11DepthStencilView* m_DepthStencilView = nullptr;

			D3D11_TEXTURE2D_DESC m_DepthStencilBufferDecs; 
			D3D11_RENDER_TARGET_VIEW_DESC m_RenderTargetViewDesc;
			D3D11_DEPTH_STENCIL_VIEW_DESC m_DepthStencilViewDesc;

			D3D11_VIEWPORT m_ViewPortDesc;

			bool m_IsBackBuffer;

			friend class PipelineController;
			friend class Renderer;
		};

	private:
		//only back buffer
		Texture(uint32_t width, uint32_t height);
		//Render target Texture
		Texture(uint32_t width, uint32_t height, int slot);						
		//General texture with image
		Texture(const std::string& path, int mySlot);
		//General textures(Texture array)
		Texture(const std::vector<std::string>& paths, int mySlot);


	public:
		void Bind(int slot) const;
		void Bind() const;

		static void MultipleTextureBind(const std::vector<std::string>& textures, int slot);
		void Resize(uint32_t Width, uint32_t Height);

	public:
		int32_t Width;
		int32_t Height;

	private:
		int32_t Channels;

		uint32_t MySlot = 0;
		uint32_t TextureArrayMax;
		bool isTextureArray;

		ID3D11Texture2D* m_Buffer = nullptr;
		ID3D11ShaderResourceView* m_ResourceView = nullptr;
		RTTInform* m_RTT = nullptr;

		friend class TextureArchive;
		friend class Renderer;
		friend class PipelineController;
	};

	class TextureArchive
	{
	private:
		static void Add(uint32_t width, uint32_t height); //for BackBuffer

	public:
		static void Add(const std::string& name, uint32_t width, uint32_t height, int slot = 1); //for Render target texture
		static void Add(const std::string& path, const std::string& name, int slot = 1);
		static void Add(const std::vector<std::string>& paths, const std::string& name, int slot = 1);
		static bool Has(const std::string& name);
		static std::shared_ptr<Texture> Get(const std::string& name);
		static void Shutdown();

		friend class Dx11Core;
	};

}

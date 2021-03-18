#pragma once

#include "Core/Base.h"
#include <stdint.h>

namespace Engine {

	class ShadowMap
	{
	public:
		ShadowMap(uint32_t width, uint32_t height);
		ShadowMap(uint32_t width, uint32_t height, uint32_t arraySize);

		void SetRenderTarget();
		void Bind(uint32_t slot);
		void Resize(uint32_t width, uint32_t height);

		static void MultipleBind(std::vector<std::shared_ptr<ShadowMap>>& shadowMaps, uint32_t count, uint32_t slot);

	private:
		int m_Width;
		int m_Height;

		ID3D11Texture2D* m_DepthStecilBuffer = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
		ID3D11DepthStencilView* m_DepthStencilView = nullptr;

		D3D11_TEXTURE2D_DESC m_DepthStencilBufferDecs;
		D3D11_SHADER_RESOURCE_VIEW_DESC m_ShaderResourceViewDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC m_DepthStencilViewDesc;

		D3D11_VIEWPORT m_ViewPortDesc;

		friend class Renderer;
	};

	class Texture
	{
	private:
		class RTTInform
		{
		public:
			RTTInform(uint32_t width, uint32_t height, ID3D11Texture2D* buffer, bool isBackBuffer = false);
			RTTInform(uint32_t width, uint32_t height, ID3D11Texture2D* buffer, uint32_t arraySize);

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
		//Only back buffer
		Texture(uint32_t width, uint32_t height, bool);
		//Render target Texture
		Texture(uint32_t width, uint32_t height);
		//Render target Textures
		Texture(uint32_t unifiedWidth, uint32_t unifiedHeight, uint32_t arraySize);
		//General texture with image
		Texture(const std::string& path);
		//General textures(Texture array)
		Texture(const std::vector<std::string>& paths, int unifiedWidth, int unifiedHeight);
		//UAV
		Texture(uint32_t width, uint32_t height, bool, bool);


	public:
		void Bind(int slot) const;
		static void UnBind(int slot);

		static void MultipleTextureBind(const std::vector<std::string>& textures, int slot);
		void Resize(int Width, int Height);
		void SetComputeOuput();
		void SetComputeResource(int slot);

	public:
		int32_t Width;
		int32_t Height;

	private:
		int32_t Channels;

		int TextureArrayMax;
		bool isTextureArray;

		ID3D11Texture2D* m_Buffer = nullptr;
		ID3D11ShaderResourceView* m_ResourceView = nullptr;
		ID3D11UnorderedAccessView* m_UAView = nullptr;
		RTTInform* m_RTT = nullptr;

		D3D11_TEXTURE2D_DESC m_TextureDesc; 
		D3D11_SHADER_RESOURCE_VIEW_DESC m_SrvDesc;

		friend class TextureArchive;
		friend class Renderer;
		friend class PipelineController;
	};

	class TextureArchive
	{
	private:
		static void CreateBackBuffer(uint32_t width, uint32_t height); //for BackBuffer

	public:
		//Create Render target texture
		static void Add(const std::string& name, uint32_t width, uint32_t height); 
		//Create Shader resource texture from image file
		static void Add(const std::string& path, const std::string& name);
		//Create Render target TextureArray if call by zero unified width or height value setted first image width or height
		static void Add(const std::string& name, uint32_t unifiedWidth, uint32_t unifiedHeight, uint32_t arraySize);
		//Create Shader Resource TextureArray if call by zero unified width or height value setted 1024
		static void Add(const std::vector<std::string>& paths, const std::string& name, uint32_t unifiedWidth, uint32_t unifiedHeight);
		//Create UAV
		static void Add(const std::string& name, uint32_t width, uint32_t height, bool, bool);

		static bool Has(const std::string& name);
		static std::shared_ptr<Texture> Get(const std::string& name);
		static std::vector<std::string> GetTextureList();
		static void Shutdown();

		friend class Dx11Core;
	};

}

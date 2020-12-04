#include "pch.h"

#include "Texture.h"
#include "Renderer/Dx11Core.h"
#include "Common/Material.h"

namespace Engine {

	Texture::RTTInform::RTTInform(uint32_t width, uint32_t height, ID3D11Texture2D* buffer, bool isBackBuffer)
		: m_IsBackBuffer(isBackBuffer)
	{
		//Create Render Target view(RTV)
		if (isBackBuffer)
		{
			Dx11Core::Get().Device->CreateRenderTargetView(buffer, NULL, &m_RenderTargetView);
			ASSERT(m_RenderTargetView, "Texture::Create back buffer view failed");
		}
		else
		{
			m_RenderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			m_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			m_RenderTargetViewDesc.Texture2D.MipSlice = 0;
			Dx11Core::Get().Device->CreateRenderTargetView(buffer, &m_RenderTargetViewDesc, &m_RenderTargetView);
			ASSERT(m_RenderTargetView, "Texture::Create Texture render target view failed");
		}

		//Depth/Stencil Buffer
		ZeroMemory(&m_DepthStencilBufferDecs, sizeof(m_DepthStencilBufferDecs));
		m_DepthStencilBufferDecs.Width = width;
		m_DepthStencilBufferDecs.Height = height;
		m_DepthStencilBufferDecs.MipLevels = 1;
		m_DepthStencilBufferDecs.ArraySize = 1;
		m_DepthStencilBufferDecs.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_DepthStencilBufferDecs.SampleDesc.Count = 1;
		m_DepthStencilBufferDecs.SampleDesc.Quality = 0;
		m_DepthStencilBufferDecs.Usage = D3D11_USAGE_DEFAULT;
		m_DepthStencilBufferDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		m_DepthStencilBufferDecs.CPUAccessFlags = 0;
		m_DepthStencilBufferDecs.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&m_DepthStencilBufferDecs, NULL, &m_DepthStecilBuffer);
		ASSERT(m_DepthStecilBuffer, "Renderer::Create DepthBuffer failed");

		//Depth/Stencil View
		ZeroMemory(&m_DepthStencilViewDesc, sizeof(m_DepthStencilViewDesc));
		m_DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		m_DepthStencilViewDesc.Texture2D.MipSlice = 0;
		Dx11Core::Get().Device->CreateDepthStencilView(m_DepthStecilBuffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
		ASSERT(m_DepthStencilView, "Renderer::Create DepthStencilViewDesc failed");

		//Viewport
		m_ViewPortDesc.Width = (float)width;
		m_ViewPortDesc.Height = (float)height;
		m_ViewPortDesc.MaxDepth = 1.0f;
		m_ViewPortDesc.MinDepth = 0.0f;
		m_ViewPortDesc.TopLeftX = 0.0f;
		m_ViewPortDesc.TopLeftY = 0.0f;
	}

	void Texture::RTTInform::Resize(uint32_t width, uint32_t height, ID3D11Texture2D* buffer)
	{
		if (m_IsBackBuffer)
		{
			Dx11Core::Get().Context->OMSetRenderTargets(0, 0, 0);
			m_RenderTargetView->Release();

			Dx11Core::Get().SwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			ID3D11Texture2D* backBuffer;
			Dx11Core::Get().SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
			ASSERT(backBuffer, "Texture::Can't get backBuffer");

			Dx11Core::Get().Device->CreateRenderTargetView(backBuffer, NULL, &m_RenderTargetView);
			backBuffer->Release();
			ASSERT(m_RenderTargetView, "Texture::Re-create RenderTargetView failed");
		}
		else
		{
		}

		if (m_DepthStecilBuffer) m_DepthStecilBuffer->Release();
		if (m_DepthStencilView) m_DepthStencilView->Release();
		m_DepthStencilBufferDecs.Width = width;
		m_DepthStencilBufferDecs.Height = height;
		Dx11Core::Get().Device->CreateTexture2D(&m_DepthStencilBufferDecs, NULL, &m_DepthStecilBuffer);
		Dx11Core::Get().Device->CreateDepthStencilView(m_DepthStecilBuffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
		if (width && height)
		{
			ASSERT(m_DepthStecilBuffer, "Texture::recreate DepthStencilBuffer fail");
			ASSERT(m_DepthStencilView, "Texture::recreate DepthStencilView fail");
		}
		m_ViewPortDesc.Width = (float)width;
		m_ViewPortDesc.Height = (float)height;
	}

	Texture::Texture(uint32_t width, uint32_t height)
		: Width(width), Height(height)
	{
		ID3D11Texture2D* backBuffer;
		Dx11Core::Get().SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		ASSERT(backBuffer, "Dx11Core::Get Backbuffer from Swapchain fail");
		m_RTT = new RTTInform(width, height, backBuffer, true);
		backBuffer->Release();
	}


	Texture::Texture(uint32_t width, uint32_t height, int slot)
		: Width(width), Height(height)
	{
		//Create texture
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Texture::Create texture failed");

		//Create Shader Resource view(SRV)
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &srvDesc, &m_ResourceView);
		ASSERT(m_ResourceView, "Texture::Create texture view failed");

		m_RTT = new RTTInform(width, height, m_Buffer, false);
	}


	Texture::Texture(const std::string & path, int mySlot)
		: MySlot(mySlot), isTextureArray(false)
	{
		LOG_MISC("Texture::Create {0} texture", path);

		//Load image with stbi
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		data = stbi_load(path.c_str(), &Width, &Height, &Channels, 4);
		uint32_t rowPitch = Width * 4 * sizeof(unsigned char);

		//Create texture
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = Width;
		textureDesc.Height = Height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Texture::Create texture failed");

		//Upload image data
		Dx11Core::Get().Context->UpdateSubresource(m_Buffer, 0, NULL, data, rowPitch, 0);
		stbi_image_free(data);

		//Create Shader Resource View(SRV)
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &srvDesc, &m_ResourceView);
		Dx11Core::Get().Context->GenerateMips(m_ResourceView);
		ASSERT(m_ResourceView, "Texture::Create texture view failed");
	}

	Texture::Texture(const std::vector<std::string>& paths, int mySlot)
		: MySlot(mySlot), isTextureArray(true)
	{
		//LOG_MISC("Texture::Create texture array");

		//int index = 0;
		//for (size_t i = 0; i < paths.size(); ++i)
		//{
		//	
		//	stbi_set_flip_vertically_on_load(1);
		//	stbi_uc* data[30];
		//	LOG_MISC("{0}",paths[i]);
		//	data[i] = stbi_load(paths[i].c_str(), &Width, &Height, &Channels, 4);
		//	if (Width != 1024) continue;

		//	if (Buffer == nullptr)
		//	{
		//		D3D11_TEXTURE2D_DESC textureDesc;
		//		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//		textureDesc.ArraySize = 21;
		//		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		//		textureDesc.CPUAccessFlags = 0;
		//		textureDesc.Width = 1024;
		//		textureDesc.Height = 1024;
		//		textureDesc.MipLevels = 0;
		//		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		//		textureDesc.SampleDesc.Count = 1;
		//		textureDesc.SampleDesc.Quality = 0;
		//		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		//		Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &Buffer);
		//		ASSERT(Buffer, "Texture::Create texture failed");
		//	}

		//	ID3D11Texture2D* temp;
		//	D3D11_TEXTURE2D_DESC textureDesc;
		//	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	textureDesc.ArraySize = 1;
		//	textureDesc.BindFlags = 0;
		//	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		//	textureDesc.Width = Width;
		//	textureDesc.Height = Height;
		//	textureDesc.MipLevels = 0;
		//	textureDesc.MiscFlags = 0;
		//	textureDesc.SampleDesc.Count = 1;
		//	textureDesc.SampleDesc.Quality = 0;
		//	textureDesc.Usage = D3D11_USAGE_STAGING;
		//	Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &temp);
		//	Dx11Core::Get().Context->UpdateSubresource(temp, 0, nullptr, data[i], Width * 4 * 4, 0);

		//	D3D11_MAPPED_SUBRESOURCE mapped;
		//	Dx11Core::Get().Context->Map(temp, 0, D3D11_MAP_READ, 0, &mapped);
		//	
		//	Dx11Core::Get().Context->UpdateSubresource(Buffer, index, nullptr, mapped.pData, mapped.RowPitch, 0);
		//	Dx11Core::Get().Context->Unmap(temp, 0);

		//	index++;

		//	//stbi_image_free(data);
		//	temp->Release();
		//}

		//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		//ZeroMemory(&srvDesc, sizeof(srvDesc));
		//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		//srvDesc.Texture2DArray.ArraySize = 21;
		//srvDesc.Texture2DArray.FirstArraySlice = 0;
		//srvDesc.Texture2DArray.MipLevels = 1;
		//srvDesc.Texture2DArray.MostDetailedMip = 0;

		//Dx11Core::Get().Device->CreateShaderResourceView(Buffer, &srvDesc, &View);
		//Dx11Core::Get().Context->GenerateMips(View);

		//ASSERT(View, "Texture::Create texture array view failed");
		//LOG_MISC("Texture::Create texture array done");
	}


	void Texture::Resize(uint32_t width, uint32_t height)
	{
		Width = width;
		Height = height;

		if (m_RTT)
			m_RTT->Resize(Width, Height, m_Buffer);
	}
	void Texture::Bind(int slot) const
	{
		Dx11Core::Get().Context->PSSetShaderResources(slot, 1, &m_ResourceView);
	}

	void Texture::Bind() const
	{
		Dx11Core::Get().Context->PSSetShaderResources(MySlot, 1, &m_ResourceView);
	}

	void Texture::MultipleTextureBind(const std::vector<std::string>& textures, int slot)
	{
		static ID3D11ShaderResourceView* views[100];

		for (size_t i = 0; i < textures.size(); ++i)
		{
			auto texture = TextureArchive::Get(textures[i]);
			if (!texture) continue;

			views[i] = texture->m_ResourceView;
		}
		Dx11Core::Get().Context->PSSetShaderResources(slot, (uint32_t)textures.size(), views);
	}

	static std::unordered_map<std::string, std::shared_ptr<Texture>> s_Textures;

	void TextureArchive::Add(uint32_t width, uint32_t height)
	{
		if (Has("BackBuffer")) return;

		s_Textures["BackBuffer"].reset(new Texture(width, height));
	}

	void TextureArchive::Add(const std::string & name, uint32_t width, uint32_t height, int slot)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(width, height, slot));
	}

	void TextureArchive::Add(const std::string & path, const std::string & name, int slot)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(path, slot));
	}

	void TextureArchive::Add(const std::vector<std::string>& paths, const std::string & name, int slot)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(paths, slot));
	}

	bool TextureArchive::Has(const std::string & name)
	{
		auto find = s_Textures.find(name);
		return find != s_Textures.end();
	}

	std::shared_ptr<Texture> TextureArchive::Get(const std::string & name)
	{
		if (!Has(name)) return nullptr;
		return s_Textures[name];
	}

	void TextureArchive::Shutdown()
	{
		s_Textures.clear();
	}

}

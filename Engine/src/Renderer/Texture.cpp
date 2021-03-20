#include "pch.h"

#include "Texture.h"
#include "Renderer/Dx11Core.h"
#include "Common/Material.h"
#include "Renderer.h"

#include "stb_image.h"
#include "stb_image_resize.h"

namespace Engine {

	ShadowMap::ShadowMap(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height), m_TextureCount(1)
	{
		//Depth/Stencil Buffer
		ZeroMemory(&m_DepthStencilBufferDecs, sizeof(m_DepthStencilBufferDecs));
		m_DepthStencilBufferDecs.Width = m_Width;
		m_DepthStencilBufferDecs.Height = m_Height;
		m_DepthStencilBufferDecs.MipLevels = 1;
		m_DepthStencilBufferDecs.ArraySize = 1;
		m_DepthStencilBufferDecs.Format = DXGI_FORMAT_R32_TYPELESS;
		m_DepthStencilBufferDecs.SampleDesc.Count = 1;
		m_DepthStencilBufferDecs.SampleDesc.Quality = 0;
		m_DepthStencilBufferDecs.Usage = D3D11_USAGE_DEFAULT;
		m_DepthStencilBufferDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		m_DepthStencilBufferDecs.CPUAccessFlags = 0;
		m_DepthStencilBufferDecs.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&m_DepthStencilBufferDecs, NULL, &m_DepthStecilBuffer);
		ASSERT(m_DepthStecilBuffer, "Renderer::Create DepthBuffer failed");

		//Depth/Stencil View
		ZeroMemory(&m_DepthStencilViewDesc, sizeof(m_DepthStencilViewDesc));
		m_DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		m_DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		m_DepthStencilViewDesc.Texture2D.MipSlice = 0;
		Dx11Core::Get().Device->CreateDepthStencilView(m_DepthStecilBuffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
		ASSERT(m_DepthStencilView, "Renderer::Create DepthStencilViewDesc failed");

		//Depth/Stencil Shader Resource view
		m_ShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		m_ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		m_ShaderResourceViewDesc.Texture2D.MipLevels = 1;
		Dx11Core::Get().Device->CreateShaderResourceView(m_DepthStecilBuffer, &m_ShaderResourceViewDesc, &m_ShaderResourceView);

		ASSERT(m_ShaderResourceView, "Texture::Create Texture render target view failed");

		//Viewport
		m_ViewPortDesc.TopLeftX = 0.0f;
		m_ViewPortDesc.TopLeftY = 0.0f;
		m_ViewPortDesc.Width = (float)m_Width;
		m_ViewPortDesc.Height = (float)m_Height;
		m_ViewPortDesc.MinDepth = 0.0f;
		m_ViewPortDesc.MaxDepth = 1.0f;
	}

	ShadowMap::ShadowMap(uint32_t width, uint32_t height, uint32_t arraySize)
		: m_Width(width), m_Height(height), m_TextureCount(arraySize)
	{
		//Depth/Stencil Buffer
		ZeroMemory(&m_DepthStencilBufferDecs, sizeof(m_DepthStencilBufferDecs));
		m_DepthStencilBufferDecs.Width = m_Width;
		m_DepthStencilBufferDecs.Height = m_Height;
		m_DepthStencilBufferDecs.MipLevels = 1;
		m_DepthStencilBufferDecs.ArraySize = arraySize;
		m_DepthStencilBufferDecs.Format = DXGI_FORMAT_R32_TYPELESS;
		m_DepthStencilBufferDecs.SampleDesc.Count = 1;
		m_DepthStencilBufferDecs.SampleDesc.Quality = 0;
		m_DepthStencilBufferDecs.Usage = D3D11_USAGE_DEFAULT;
		m_DepthStencilBufferDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		m_DepthStencilBufferDecs.CPUAccessFlags = 0;
		if(arraySize == 6) m_DepthStencilBufferDecs.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		Dx11Core::Get().Device->CreateTexture2D(&m_DepthStencilBufferDecs, NULL, &m_DepthStecilBuffer);
		ASSERT(m_DepthStecilBuffer, "Renderer::Create DepthBuffer failed");

		//Depth/Stencil View
		ZeroMemory(&m_DepthStencilViewDesc, sizeof(m_DepthStencilViewDesc));
		m_DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		m_DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		m_DepthStencilViewDesc.Texture2DArray.ArraySize = arraySize;
		m_DepthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
		m_DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
		Dx11Core::Get().Device->CreateDepthStencilView(m_DepthStecilBuffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
		ASSERT(m_DepthStencilView, "Renderer::Create DepthStencilViewDesc failed");

		//Depth/Stencil Shader Resource view
		m_ShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		if (arraySize == 6)
		{
			m_ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			m_ShaderResourceViewDesc.TextureCube.MipLevels = 1;
			m_ShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		}
		else
		{
			m_ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			m_ShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
			m_ShaderResourceViewDesc.Texture2DArray.ArraySize = arraySize;
			m_ShaderResourceViewDesc.Texture2DArray.MipLevels = 1;
			m_ShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		}
		Dx11Core::Get().Device->CreateShaderResourceView(m_DepthStecilBuffer, &m_ShaderResourceViewDesc, &m_ShaderResourceView);
		ASSERT(m_ShaderResourceView, "Texture::Create Texture render target view failed");

		//Viewport
		m_ViewPortDesc.Width = (float)m_Width;
		m_ViewPortDesc.Height = (float)m_Height;
		m_ViewPortDesc.TopLeftX = 0.0f;
		m_ViewPortDesc.TopLeftY = 0.0f;
		m_ViewPortDesc.MaxDepth = 1.0f;
		m_ViewPortDesc.MinDepth = 0.0f;

	}

	void ShadowMap::SetRenderTarget()
	{
		ID3D11RenderTargetView* nullTarget = nullptr;
		Dx11Core::Get().Context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
		D3D11_VIEWPORT vp[6] = { m_ViewPortDesc , m_ViewPortDesc , m_ViewPortDesc , m_ViewPortDesc , m_ViewPortDesc , m_ViewPortDesc };
		Dx11Core::Get().Context->RSSetViewports(m_TextureCount, vp);
		Dx11Core::Get().Context->OMSetRenderTargets(1, &nullTarget, m_DepthStencilView);
	}

	void ShadowMap::Bind(uint32_t slot)
	{
		Dx11Core::Get().Context->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
	}

	void ShadowMap::Resize(uint32_t width, uint32_t height)
	{
		if (m_DepthStecilBuffer) m_DepthStecilBuffer->Release();
		if (m_DepthStencilView) m_DepthStencilView->Release();
		if (m_ShaderResourceView) m_ShaderResourceView->Release();

		m_Width = width;
		m_Height = height;

		m_DepthStencilBufferDecs.Width = m_Width;
		m_DepthStencilBufferDecs.Height = m_Height;

		Dx11Core::Get().Device->CreateTexture2D(&m_DepthStencilBufferDecs, NULL, &m_DepthStecilBuffer);
		Dx11Core::Get().Device->CreateDepthStencilView(m_DepthStecilBuffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
		Dx11Core::Get().Device->CreateShaderResourceView(m_DepthStecilBuffer, &m_ShaderResourceViewDesc, &m_ShaderResourceView);
		ASSERT(m_ShaderResourceView || m_DepthStecilBuffer || m_DepthStecilBuffer, "Texture::Resize ShadowBuffer failed");
	}

	void ShadowMap::MultipleBind(std::vector<std::shared_ptr<ShadowMap>>& shadowMaps, uint32_t count, uint32_t slot)
	{
		ID3D11ShaderResourceView** array = nullptr;
		for (uint32_t i = 0; i < count; ++i)
			array[i] = shadowMaps[i]->m_ShaderResourceView;

		Dx11Core::Get().Context->PSSetShaderResources(slot, count, array);
	}

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

	Texture::RTTInform::RTTInform(uint32_t width, uint32_t height, ID3D11Texture2D * buffer, uint32_t arraySize)
	{
		//Render Target View
		ZeroMemory(&m_RenderTargetViewDesc, sizeof(m_RenderTargetViewDesc));
		m_RenderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		m_RenderTargetViewDesc.Texture2DArray.ArraySize = arraySize;
		m_RenderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
		m_RenderTargetViewDesc.Texture2DArray.MipSlice = 0;
		Dx11Core::Get().Device->CreateRenderTargetView(buffer, &m_RenderTargetViewDesc, &m_RenderTargetView);
		ASSERT(m_RenderTargetView, "Texture::Create Texture render target view failed");

		//Depth/Stencil Buffer
		ZeroMemory(&m_DepthStencilBufferDecs, sizeof(m_DepthStencilBufferDecs));
		m_DepthStencilBufferDecs.Width = width;
		m_DepthStencilBufferDecs.Height = height;
		m_DepthStencilBufferDecs.MipLevels = 1;
		m_DepthStencilBufferDecs.ArraySize = arraySize;
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
		m_DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		m_DepthStencilViewDesc.Texture2DArray.ArraySize = arraySize;
		m_DepthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
		m_DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
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

	//Texture::RTTInform::RTTInform(uint32_t width, uint32_t height, ID3D11Texture2D * buffer, DXGI_FORMAT format)
	//{
	//	m_RenderTargetViewDesc.Format = format;
	//	m_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//	m_RenderTargetViewDesc.Texture2D.MipSlice = 0;
	//	Dx11Core::Get().Device->CreateRenderTargetView(buffer, &m_RenderTargetViewDesc, &m_RenderTargetView);
	//	ASSERT(m_RenderTargetView, "Texture::Create Texture render target view failed");

	//	//Depth/Stencil Buffer
	//	ZeroMemory(&m_DepthStencilBufferDecs, sizeof(m_DepthStencilBufferDecs));
	//	m_DepthStencilBufferDecs.Width = width;
	//	m_DepthStencilBufferDecs.Height = height;
	//	m_DepthStencilBufferDecs.MipLevels = 1;
	//	m_DepthStencilBufferDecs.ArraySize = 1;
	//	m_DepthStencilBufferDecs.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	m_DepthStencilBufferDecs.SampleDesc.Count = 1;
	//	m_DepthStencilBufferDecs.SampleDesc.Quality = 0;
	//	m_DepthStencilBufferDecs.Usage = D3D11_USAGE_DEFAULT;
	//	m_DepthStencilBufferDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//	m_DepthStencilBufferDecs.CPUAccessFlags = 0;
	//	m_DepthStencilBufferDecs.MiscFlags = 0;
	//	Dx11Core::Get().Device->CreateTexture2D(&m_DepthStencilBufferDecs, NULL, &m_DepthStecilBuffer);
	//	ASSERT(m_DepthStecilBuffer, "Renderer::Create DepthBuffer failed");

	//	//Depth/Stencil View
	//	ZeroMemory(&m_DepthStencilViewDesc, sizeof(m_DepthStencilViewDesc));
	//	m_DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	m_DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//	m_DepthStencilViewDesc.Texture2D.MipSlice = 0;
	//	Dx11Core::Get().Device->CreateDepthStencilView(m_DepthStecilBuffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
	//	ASSERT(m_DepthStencilView, "Renderer::Create DepthStencilViewDesc failed");

	//	//Viewport
	//	m_ViewPortDesc.Width = (float)width;
	//	m_ViewPortDesc.Height = (float)height;
	//	m_ViewPortDesc.MaxDepth = 1.0f;
	//	m_ViewPortDesc.MinDepth = 0.0f;
	//	m_ViewPortDesc.TopLeftX = 0.0f;
	//	m_ViewPortDesc.TopLeftY = 0.0f;
	//}

	void Texture::RTTInform::Resize(uint32_t width, uint32_t height, ID3D11Texture2D* buffer)
	{
		Dx11Core::Get().Context->OMSetRenderTargets(0, 0, 0);
		m_RenderTargetView->Release();

		if (m_IsBackBuffer)
		{
			Dx11Core::Get().SwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			ID3D11Texture2D* backBuffer;
			Dx11Core::Get().SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
			ASSERT(backBuffer, "Texture::Can't get backBuffer");

			Dx11Core::Get().Device->CreateRenderTargetView(backBuffer, NULL, &m_RenderTargetView);
			backBuffer->Release();
			ASSERT(m_RenderTargetView, "Texture::Re-create BackBuffer RenderTargetView failed");
		}
		else
		{
			Dx11Core::Get().Device->CreateRenderTargetView(buffer, NULL, &m_RenderTargetView);
			ASSERT(m_RenderTargetView, "Texture::Re-create RenderTargetView failed");
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

	Texture::Texture(uint32_t width, uint32_t height, bool)
		: Width(width), Height(height)
	{
		ID3D11Texture2D* backBuffer;
		Dx11Core::Get().SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		ASSERT(backBuffer, "Dx11Core::Get Backbuffer from Swapchain fail");
		m_RTT = new RTTInform(width, height, backBuffer, true);
		backBuffer->Release();
	}


	Texture::Texture(uint32_t width, uint32_t height)
		: Width(width), Height(height)
	{
		//Create texture
		m_TextureDesc.Width = width;
		m_TextureDesc.Height = height;
		m_TextureDesc.MipLevels = 1;
		m_TextureDesc.ArraySize = 1;
		m_TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_TextureDesc.SampleDesc.Count = 1;
		m_TextureDesc.SampleDesc.Quality = 0;
		m_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		m_TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		m_TextureDesc.CPUAccessFlags = 0;
		m_TextureDesc.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&m_TextureDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Texture::Create texture failed");

		//Create Shader Resource view(SRV)
		ZeroMemory(&m_SrvDesc, sizeof(m_SrvDesc));
		m_SrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_SrvDesc.Texture2D.MostDetailedMip = 0;
		m_SrvDesc.Texture2D.MipLevels = 1;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_SrvDesc, &m_ResourceView);
		ASSERT(m_ResourceView, "Texture::Create texture view failed");

		m_RTT = new RTTInform(width, height, m_Buffer);
	}

	Texture::Texture(uint32_t unifiedWidth, uint32_t unifiedHeight, uint32_t arraySize)
	{
		//Create texture
		m_TextureDesc.Width = unifiedWidth;
		m_TextureDesc.Height = unifiedHeight;
		m_TextureDesc.MipLevels = 1;
		m_TextureDesc.ArraySize = arraySize;
		m_TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_TextureDesc.SampleDesc.Count = 1;
		m_TextureDesc.SampleDesc.Quality = 0;
		m_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		m_TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		m_TextureDesc.CPUAccessFlags = 0;
		m_TextureDesc.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&m_TextureDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Texture::Create texture failed");

		//Create Shader Resource view(SRV)
		m_SrvDesc.Format = m_TextureDesc.Format;
		m_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		m_SrvDesc.Texture2DArray.ArraySize = arraySize;
		m_SrvDesc.Texture2DArray.FirstArraySlice = 0;
		m_SrvDesc.Texture2DArray.MipLevels = 1;
		m_SrvDesc.Texture2DArray.MostDetailedMip = 0;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_SrvDesc, &m_ResourceView);
		ASSERT(m_ResourceView, "Texture::Create texture view failed");

		m_RTT = new RTTInform(unifiedWidth, unifiedHeight, m_Buffer, arraySize);
	}

	Texture::Texture(const std::string & path)
		: isTextureArray(false)
	{
		LOG_MISC("Texture::Create {0} texture", path);

		//Load image with stbi
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		data = stbi_load(path.c_str(), &Width, &Height, &Channels, 4);
		uint32_t rowPitch = Width * 4 * sizeof(unsigned char);

		//Create texture
		m_TextureDesc.Width = Width;
		m_TextureDesc.Height = Height;
		m_TextureDesc.MipLevels = 0;
		m_TextureDesc.ArraySize = 1;
		m_TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_TextureDesc.SampleDesc.Count = 1;
		m_TextureDesc.SampleDesc.Quality = 0;
		m_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		m_TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		m_TextureDesc.CPUAccessFlags = 0;
		m_TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		Dx11Core::Get().Device->CreateTexture2D(&m_TextureDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Texture::Create texture failed");

		//Upload image data
		Dx11Core::Get().Context->UpdateSubresource(m_Buffer, 0, NULL, data, rowPitch, 0);
		stbi_image_free(data);

		//Create Shader Resource View(SRV)
		m_SrvDesc.Format = m_TextureDesc.Format;
		m_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_SrvDesc.Texture2D.MostDetailedMip = 0;
		m_SrvDesc.Texture2D.MipLevels = 1;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_SrvDesc, &m_ResourceView);
		Dx11Core::Get().Context->GenerateMips(m_ResourceView);
		ASSERT(m_ResourceView, "Texture::Create texture view failed");
	}

	Texture::Texture(const std::vector<std::string>& paths, int unifiedWidth, int unifiedHeight)
		: isTextureArray(true), Width(unifiedWidth), Height(unifiedHeight)
	{
		int maxMiplevel;
		for (size_t i = 0; i < paths.size(); ++i)
		{
			stbi_set_flip_vertically_on_load(1);
			
			int width, height;
			stbi_uc* data = stbi_load(paths[i].c_str(), &width, &height, &Channels, 4);
			
			if (Width == 0) Width = width;
			if (Height == 0) Height = height;

			stbi_uc* resizedData = new stbi_uc[Width * Height * 4];

			if (Width != width || Height != height)
				stbir_resize_uint8_srgb(data, width, height, 0, resizedData, Width, Height, 0, 4, STBIR_FLAG_ALPHA_USES_COLORSPACE, 0);
			
			if (m_Buffer == nullptr)
			{
				LOG_TRACE("Texture::Create texture array unified width {0} height {1}", Width, Height);

				m_TextureDesc.Width = Width;
				m_TextureDesc.Height = Height;
				m_TextureDesc.MipLevels = 0;
				m_TextureDesc.ArraySize = (uint32_t)paths.size();
				m_TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				m_TextureDesc.SampleDesc.Count = 1;
				m_TextureDesc.SampleDesc.Quality = 0;
				m_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
				m_TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				m_TextureDesc.CPUAccessFlags = 0;
				m_TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
				Dx11Core::Get().Device->CreateTexture2D(&m_TextureDesc, NULL, &m_Buffer);
				ASSERT(m_Buffer, "Texture::Create texture failed");

				m_Buffer->GetDesc(&m_TextureDesc);
				maxMiplevel = m_TextureDesc.MipLevels;
			}
			LOG_MISC("{0}", paths[i]);
			Dx11Core::Get().Context->UpdateSubresource(m_Buffer, i * maxMiplevel, NULL, resizedData, UINT(Width * 4 * sizeof(unsigned char)), 0);
			stbi_image_free(data);
			delete[] resizedData;
		}

		m_SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		m_SrvDesc.Texture2DArray.MostDetailedMip = 0;
		m_SrvDesc.Texture2DArray.MipLevels = maxMiplevel;
		m_SrvDesc.Texture2DArray.FirstArraySlice = 0;
		m_SrvDesc.Texture2DArray.ArraySize = (UINT)paths.size();
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_SrvDesc, &m_ResourceView);
		Dx11Core::Get().Context->GenerateMips(m_ResourceView);
	}

	Texture::Texture(uint32_t width, uint32_t height, bool, bool)
		: Width(width), Height(height)
	{
		//Create texture
		m_TextureDesc.Width = Width;
		m_TextureDesc.Height = Height;
		m_TextureDesc.MipLevels = 1;
		m_TextureDesc.ArraySize = 1;
		m_TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_TextureDesc.SampleDesc.Count = 1;
		m_TextureDesc.SampleDesc.Quality = 0;
		m_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		m_TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		m_TextureDesc.CPUAccessFlags = 0;
		m_TextureDesc.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&m_TextureDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Texture::Create texture failed");

		m_SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_SrvDesc.Texture2D.MipLevels = 1;
		m_SrvDesc.Texture2D.MostDetailedMip = 0;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_SrvDesc, &m_ResourceView);
		ASSERT(m_ResourceView, "Texture::Create texture failed");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		Dx11Core::Get().Device->CreateUnorderedAccessView(m_Buffer, &uavDesc, &m_UAView);
		ASSERT(m_UAView, "Texture::Create texture failed");
	}

	void Texture::Resize(int width, int height)
	{
		Width = width;
		Height = height;

		if (m_ResourceView)
		{
			m_Buffer->Release();
			m_ResourceView->Release();

			m_TextureDesc.Width = Width;
			m_TextureDesc.Height = Height;

			Dx11Core::Get().Device->CreateTexture2D(&m_TextureDesc, NULL, &m_Buffer);
			ASSERT(m_Buffer, "Texture::Resize texture failed");

			Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_SrvDesc, &m_ResourceView);
			ASSERT(m_ResourceView, "Texture::Resize texture failed");
		}

		if (m_RTT) {}
			m_RTT->Resize(Width, Height, m_Buffer);
	}

	void Texture::SetComputeOuput()
	{
		Dx11Core::Get().Context->CSSetUnorderedAccessViews(0, 1, &m_UAView, NULL);
	}

	void Texture::SetComputeResource(int slot)
	{
		Dx11Core::Get().Context->CSSetShaderResources(slot, 1, &m_ResourceView);
	}

	void Texture::Bind(int slot) const
	{
		Dx11Core::Get().Context->PSSetShaderResources(slot, 1, &m_ResourceView);
	}
	void Texture::UnBind(int slot)
	{
		ID3D11ShaderResourceView* null = nullptr;
		Dx11Core::Get().Context->PSSetShaderResources(slot, 1, &null);
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

	void TextureArchive::CreateBackBuffer(uint32_t width, uint32_t height)
	{
		if (Has("BackBuffer")) return;

		s_Textures["BackBuffer"].reset(new Texture(width, height, true));
	}

	void TextureArchive::Add(const std::string & name, uint32_t width, uint32_t height)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(width, height));
	}

	void TextureArchive::Add(const std::string & path, const std::string & name)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(path));
	}

	void TextureArchive::Add(const std::string & name, uint32_t unifiedWidth, uint32_t unifiedHeight, uint32_t arraySize)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(unifiedWidth, unifiedHeight, arraySize));
	}

	void TextureArchive::Add(const std::vector<std::string>& paths, const std::string & name, uint32_t unifiedWidth, uint32_t unifiedHeight)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(paths, unifiedWidth, unifiedWidth));
	}

	void TextureArchive::Add(const std::string & name, uint32_t width, uint32_t height, bool, bool)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(width, height, true, true));
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

	std::vector<std::string> TextureArchive::GetTextureList()
	{
		std::vector<std::string> ret;
		for (auto&[name, texture]: s_Textures)
			ret.push_back(name);
		return ret;
	}

}

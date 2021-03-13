#include "pch.h"

#include "GBuffer.h"
#include "Dx11Core.h"

namespace Engine {
	
	GBuffer::DepthElem::DepthElem(uint32_t width, uint32_t height)
	{
		//Depth/Stencil Buffer
		ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
		m_BufferDesc.Width = width;
		m_BufferDesc.Height = height;
		m_BufferDesc.MipLevels = 1;
		m_BufferDesc.ArraySize = 1;
		m_BufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		m_BufferDesc.SampleDesc.Count = 1;
		m_BufferDesc.SampleDesc.Quality = 0;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&m_BufferDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Renderer::Create DepthBuffer failed");

		//Depth/Stencil Shader Resource View
		ZeroMemory(&m_ResourceViewDesc, sizeof(m_ResourceViewDesc));
		m_ResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		m_ResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_ResourceViewDesc.Texture2D.MostDetailedMip = 0;
		m_ResourceViewDesc.Texture2D.MipLevels = 1;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_ResourceViewDesc, &m_ResourceView);
		ASSERT(m_ResourceView, "Renderer::Create DepthStencil Shader Resource View failed")

		//Depth/Stencil View
		ZeroMemory(&m_DepthStencilViewDesc, sizeof(m_DepthStencilViewDesc));
		m_DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		m_DepthStencilViewDesc.Texture2D.MipSlice = 0;
		Dx11Core::Get().Device->CreateDepthStencilView(m_Buffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
		ASSERT(m_DepthStencilView, "Renderer::Create DepthStencilViewDesc failed");
	}

	void GBuffer::DepthElem::Resize(uint32_t width, uint32_t height)
	{
		if (m_Buffer) m_Buffer->Release();
		if (m_DepthStencilView) m_DepthStencilView->Release();
		if (m_ResourceView) m_ResourceView->Release();

		m_BufferDesc.Width = width;
		m_BufferDesc.Height = height;
		Dx11Core::Get().Device->CreateTexture2D(&m_BufferDesc, NULL, &m_Buffer);
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_ResourceViewDesc, &m_ResourceView);
		Dx11Core::Get().Device->CreateDepthStencilView(m_Buffer, &m_DepthStencilViewDesc, &m_DepthStencilView);
		ASSERT(m_Buffer, "Renderer::Create DepthBuffer failed");
		ASSERT(m_ResourceView, "Renderer::Create DepthStencil Shader Resource View failed");
		ASSERT(m_DepthStencilView, "Renderer::Create DepthStencilViewDesc failed");

	}

	GBuffer::BufferElem::BufferElem(uint32_t width, uint32_t height, DXGI_FORMAT fomat)
	{
		//Texture Buffer
		ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
		m_BufferDesc.Width = width;
		m_BufferDesc.Height = height;
		m_BufferDesc.MipLevels = 1;
		m_BufferDesc.ArraySize = 1;
		m_BufferDesc.Format = fomat;
		m_BufferDesc.SampleDesc.Count = 1;
		m_BufferDesc.SampleDesc.Quality = 0;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		auto ret = Dx11Core::Get().Device->CreateTexture2D(&m_BufferDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Renderer::Create Buffer failed");

		//Depth/Stencil Shader Resource View
		ZeroMemory(&m_ResourceViewDesc, sizeof(m_ResourceViewDesc));
		m_ResourceViewDesc.Format = fomat;
		m_ResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_ResourceViewDesc.Texture2D.MostDetailedMip = 0;
		m_ResourceViewDesc.Texture2D.MipLevels = 1;
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_ResourceViewDesc, &m_ResourceView);
		ASSERT(m_ResourceView, "Renderer::Create Shader Resource View failed")

		//Depth/Stencil Render Target View
		ZeroMemory(&m_RenderTargetViewDesc, sizeof(m_RenderTargetViewDesc));
		m_RenderTargetViewDesc.Format = fomat;
		m_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		m_RenderTargetViewDesc.Texture2D.MipSlice = 0;
		Dx11Core::Get().Device->CreateRenderTargetView(m_Buffer, &m_RenderTargetViewDesc, &m_RenderTargetView);
		ASSERT(m_RenderTargetView, "Renderer::Create Render Target View failed");
	}

	void GBuffer::BufferElem::Resize(uint32_t width, uint32_t height)
	{
		if (m_Buffer) m_Buffer->Release();
		if (m_RenderTargetView) m_RenderTargetView->Release();
		if (m_ResourceView) m_ResourceView->Release();

		m_BufferDesc.Width = width;
		m_BufferDesc.Height = height;
		
		Dx11Core::Get().Device->CreateTexture2D(&m_BufferDesc, NULL, &m_Buffer);
		ASSERT(m_Buffer, "Renderer::Create DepthBuffer failed");
		
		Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &m_ResourceViewDesc, &m_ResourceView);
		ASSERT(m_ResourceView, "Renderer::Create DepthStencil Shader Resource View failed");
		
		Dx11Core::Get().Device->CreateRenderTargetView(m_Buffer, &m_RenderTargetViewDesc, &m_RenderTargetView);
		ASSERT(m_RenderTargetView, "Renderer::Create DepthStencilViewDesc failed");
	}

	GBuffer::GBuffer(uint32_t width, uint32_t height, const std::initializer_list<std::pair<std::string, DXGI_FORMAT>>& items)
		: m_Width(width), m_Height(height)
	{
		m_DepthBuffer = new DepthElem(m_Width, m_Height);

		for (auto&[name, format] : items)
		{
			m_BindingOrder.emplace_back(name);
			m_Buffers[name] = new BufferElem(m_Width, m_Height, format);
		}

		m_ViewPortDesc.Width = (float)m_Width;
		m_ViewPortDesc.Height = (float)m_Height;
		m_ViewPortDesc.MinDepth = 0.0f;
		m_ViewPortDesc.MaxDepth = 1.0f;
		m_ViewPortDesc.TopLeftX = 0.0f;
		m_ViewPortDesc.TopLeftY = 0.0f;
	}

	GBuffer::~GBuffer()
	{
		m_DepthBuffer->m_Buffer->Release();
		m_DepthBuffer->m_DepthStencilView->Release();
		m_DepthBuffer->m_ResourceView->Release();
		delete m_DepthBuffer;

		for (auto[name, buffer] : m_Buffers)
		{
			buffer->m_Buffer->Release();
			buffer->m_ResourceView->Release();
			buffer->m_RenderTargetView->Release();
			delete buffer;
		}

		m_Buffers.clear();
	}
	
	void GBuffer::Bind()
	{
		ID3D11ShaderResourceView* views[8];
		views[0] = m_DepthBuffer->m_ResourceView;
		for (size_t i = 1; i <= m_BindingOrder.size(); ++i)
			views[i] = m_Buffers[m_BindingOrder[i - 1]]->m_ResourceView;

		Dx11Core::Get().Context->PSSetShaderResources(0, uint32_t(m_BindingOrder.size() + 1), views);
	}

	void GBuffer::UnBind()
	{
		ID3D11RenderTargetView* view[8]{ 0, };
		ID3D11DepthStencilView* depth = nullptr;
		Dx11Core::Get().Context->OMSetRenderTargets((uint32_t)m_BindingOrder.size(), view, depth);
	}

	void GBuffer::SetRenderTarget()
	{
		Dx11Core::Get().Context->ClearDepthStencilView(m_DepthBuffer->m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* views[8];
		float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for (size_t i = 0; i < m_BindingOrder.size(); ++i)
		{
			auto& renderTargetVeiw = m_Buffers[m_BindingOrder[i]]->m_RenderTargetView;
			Dx11Core::Get().Context->ClearRenderTargetView(renderTargetVeiw, color);
			views[i] = m_Buffers[m_BindingOrder[i]]->m_RenderTargetView;
		}
		
		Dx11Core::Get().Context->OMSetRenderTargets((uint32_t)m_BindingOrder.size(), views, m_DepthBuffer->m_DepthStencilView);
		Dx11Core::Get().Context->RSSetViewports(1, &m_ViewPortDesc);
	}

	void GBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		m_DepthBuffer->Resize(m_Width, m_Height);
		for (auto&[name, buffer] : m_Buffers)
			buffer->Resize(m_Width, m_Height);

		m_ViewPortDesc.Width = (float)m_Width;
		m_ViewPortDesc.Height = (float)m_Height;
	}
}


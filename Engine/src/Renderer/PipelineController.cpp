#include "pch.h"

#include "PipelineController.h"
#include "Core/ModuleCore.h"
#include "Dx11Core.h"
#include "Texture.h"

namespace Engine {

	void PipelineController::Init(const WindowProp& prop)
	{
		m_DepthStencil.Init(prop);
		m_Rasterlizer.Init();
		m_Blend.Init();
	}

	
	PipelineController& PipelineController::Bind(PipelineComponent comp)
	{
		switch (comp)
		{
		case PipelineComponent::DepthStencil: Dx11Core::Get().Context->OMSetRenderTargets(1, &Dx11Core::Get().RenderTargetView, m_DepthStencil.View);
		}
		Pipeline.emplace(comp);
		return *this;
	}

	PipelineController& PipelineController::Unbind(PipelineComponent comp)
	{
		switch (comp)
		{
		case PipelineComponent::DepthStencil: break;
		}
		auto target = Pipeline.find(comp);
		if (target != Pipeline.end())
		{
			Pipeline.erase(target);
		}
		return *this;
	}

	PipelineController& PipelineController::SetDepthStencil(DepthStencilOpt opt)
	{
		switch (opt)
		{
		case DepthStencilOpt::Enable: Dx11Core::Get().Context->OMSetDepthStencilState(m_DepthStencil.Enable, 1); 
			LOG_MISC("Renderer::Set depthstencil enable"); break;
		case DepthStencilOpt::Disable: Dx11Core::Get().Context->OMSetDepthStencilState(m_DepthStencil.Disable, 1); 
			LOG_MISC("Renderer::Set depthstencil disable"); break;
		}

		m_DepthStencil.opt = opt;
		return *this;
	}

	PipelineController& PipelineController::SetBlend(BlendOpt opt)
	{
		switch (opt)
		{
		case BlendOpt::Alpha: Dx11Core::Get().Context->OMSetBlendState(m_Blend.AlphaBlend, m_Blend.BlendFactor, 1);
			LOG_MISC("Renderer::Set Blend state Alpha");  break;
		case BlendOpt::None: Dx11Core::Get().Context->OMSetBlendState(m_Blend.None, m_Blend.BlendFactor, 0xffffffff); 
			LOG_MISC("Renderer::Set Blend state none"); break;
		}
		m_Blend.opt = opt;
		return *this;
	}

	PipelineController& PipelineController::SetRasterize(RasterlizerOpt opt)
	{
		switch (opt)
		{
		case RasterlizerOpt::Solid: Dx11Core::Get().Context->RSSetState(m_Rasterlizer.Solid); 
			LOG_MISC("Renderer::Set Rasterize state Fill solid"); break;
		case RasterlizerOpt::Wire: Dx11Core::Get().Context->RSSetState(m_Rasterlizer.Wire); 
			LOG_MISC("Renderer::Set Rasterize state Fill wire"); break;
		}
		m_Rasterlizer.opt;
		return *this;
	}

	PipelineController & PipelineController::SetRenderTarget(const std::string & targetTextureName)
	{
		if(targetTextureName == "BackBuffer") 
			Dx11Core::Get().Context->OMSetRenderTargets(1, &Dx11Core::Get().RenderTargetView, m_DepthStencil.View);
		else if(TextureArchive::Has(targetTextureName))
		{
			ASSERT(TextureArchive::Get(targetTextureName)->RenderTargetView != nullptr, "target texture has no render target view");
			m_UsagedRTT.insert(targetTextureName);
			Dx11Core::Get().Context->OMSetRenderTargets(1, &TextureArchive::Get(targetTextureName)->RenderTargetView, m_DepthStencil.View);
		}
		return *this;
	}

	void PipelineController::ClearRTT()
	{
		float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for (auto& texture : m_UsagedRTT)
		{
			Dx11Core::Get().Context->ClearRenderTargetView(TextureArchive::Get(texture)->RenderTargetView, clearColor);
		}
		m_UsagedRTT.clear();
	}

	void PipelineController::DepthStencil::Resize()
	{
		DepthBufferDecs.Width = Dx11Core::Get().WinProp->Width;
		DepthBufferDecs.Height = Dx11Core::Get().WinProp->Height;

		Dx11Core::Get().Device->CreateTexture2D(&DepthBufferDecs, NULL, &Buffer);
		Dx11Core::Get().Device->CreateDepthStencilView(Buffer, &DepthStencilViewDesc, &View);
	}

	void PipelineController::DepthStencil::Init(const WindowProp& prop)
	{
		//Depth/Stencil buffer
		ZeroMemory(&DepthBufferDecs, sizeof(DepthBufferDecs));
		DepthBufferDecs.Width = prop.Width;
		DepthBufferDecs.Height = prop.Width;
		DepthBufferDecs.MipLevels = 1;
		DepthBufferDecs.ArraySize = 1;
		DepthBufferDecs.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DepthBufferDecs.SampleDesc.Count = 1;
		DepthBufferDecs.SampleDesc.Quality = 0;
		DepthBufferDecs.Usage = D3D11_USAGE_DEFAULT;
		DepthBufferDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DepthBufferDecs.CPUAccessFlags = 0;
		DepthBufferDecs.MiscFlags = 0;
		Dx11Core::Get().Device->CreateTexture2D(&DepthBufferDecs, NULL, &Buffer);
		ASSERT(Buffer, "Renderer::Create DepthBuffer failed");

		//Depth/Stencil View
		ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

		DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DepthStencilViewDesc.Texture2D.MipSlice = 0;

		Dx11Core::Get().Device->CreateDepthStencilView(Buffer, &DepthStencilViewDesc, &View);
		ASSERT(View, "Renderer::Create DepthStencilViewDesc failed");

		//Depth/Stencil State
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		Dx11Core::Get().Device->CreateDepthStencilState(&depthStencilDesc, &Enable);

		depthStencilDesc.DepthEnable = false;
		Dx11Core::Get().Device->CreateDepthStencilState(&depthStencilDesc, &Disable);
	}

	void PipelineController::Rasterlizer::Init()
	{
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		Dx11Core::Get().Device->CreateRasterizerState(&rasterDesc, &Solid);

		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
		Dx11Core::Get().Device->CreateRasterizerState(&rasterDesc, &Wire);
	}

	void PipelineController::Blend::Init()
	{
		D3D11_BLEND_DESC blendStateDesc;
		ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
		blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		Dx11Core::Get().Device->CreateBlendState(&blendStateDesc, &AlphaBlend);
	}

}

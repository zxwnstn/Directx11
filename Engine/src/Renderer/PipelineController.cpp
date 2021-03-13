#include "pch.h"

#include "PipelineController.h"
#include "Core/ModuleCore.h"
#include "Dx11Core.h"
#include "Texture.h"

namespace Engine {

	void PipelineController::Init(const WindowProp& prop)
	{
		m_DepthStencil.Init();
		m_Rasterlizer.Init();
		m_Blend.Init();
	}

	PipelineController& PipelineController::SetDepthStencil(DepthStencilOpt opt)
	{
		switch (opt)
		{
		case DepthStencilOpt::Enable: Dx11Core::Get().Context->OMSetDepthStencilState(m_DepthStencil.Enable, 1); break;
		case DepthStencilOpt::Disable: Dx11Core::Get().Context->OMSetDepthStencilState(m_DepthStencil.Disable, 1); break;
		}

		m_DepthStencil.opt = opt;
		return *this;
	}

	PipelineController& PipelineController::SetBlend(BlendOpt opt)
	{
		switch (opt)
		{
		case BlendOpt::Alpha: Dx11Core::Get().Context->OMSetBlendState(m_Blend.AlphaBlend, m_Blend.BlendFactor, 1); break;
		case BlendOpt::GBuffer: Dx11Core::Get().Context->OMSetBlendState(m_Blend.GBufferBlend, m_Blend.BlendFactor, 0xffffffff); break;
		case BlendOpt::None: Dx11Core::Get().Context->OMSetBlendState(m_Blend.None, m_Blend.BlendFactor, 0xffffffff); break;
		}
		m_Blend.opt = opt;
		return *this;
	}

	PipelineController& PipelineController::SetRasterize(RasterlizerOpt opt)
	{
		switch (opt)
		{
		case RasterlizerOpt::Solid: Dx11Core::Get().Context->RSSetState(m_Rasterlizer.Solid); break;
		case RasterlizerOpt::Wire: Dx11Core::Get().Context->RSSetState(m_Rasterlizer.Wire); break;
		case RasterlizerOpt::Shadow: Dx11Core::Get().Context->RSSetState(m_Rasterlizer.Shadow); break;
		}
		m_Rasterlizer.opt;
		return *this;
	}

	PipelineController & PipelineController::SetRenderTarget(const std::string & targetTextureName)
	{
		if (!TextureArchive::Has(targetTextureName))
		{
			LOG_WARN("Renderer::RenderTarget {0} Dose not exist!", targetTextureName);
			return *this;
		}

		auto RttInform = TextureArchive::Get(targetTextureName)->m_RTT;
		ASSERT(RttInform->m_RenderTargetView, "Renderer::RenderTarget has no RenderTargetView");

		m_UsagedRTT.insert(targetTextureName);
		Dx11Core::Get().Context->OMSetRenderTargets(1, &RttInform->m_RenderTargetView, RttInform->m_DepthStencilView);
		Dx11Core::Get().Context->RSSetViewports(1, &RttInform->m_ViewPortDesc);

		return *this;
	}

	void PipelineController::ClearRTT(const std::string & target)
	{
		float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		if (!target.empty())
		{
			auto RttInform = TextureArchive::Get(target)->m_RTT;
			Dx11Core::Get().Context->ClearRenderTargetView(RttInform->m_RenderTargetView, clearColor);
			Dx11Core::Get().Context->ClearDepthStencilView(RttInform->m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}
		else
		{
			for (auto& textureName : m_UsagedRTT)
			{
				auto RttInform = TextureArchive::Get(textureName)->m_RTT;
				Dx11Core::Get().Context->ClearRenderTargetView(RttInform->m_RenderTargetView, clearColor);
				Dx11Core::Get().Context->ClearDepthStencilView(RttInform->m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
			}
			m_UsagedRTT.clear();
		}
	}

	void PipelineController::DepthStencil::Init()
	{
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
		ASSERT(Enable, "PipelineController::Create DepthStencilState fail");

		depthStencilDesc.DepthEnable = false;
		Dx11Core::Get().Device->CreateDepthStencilState(&depthStencilDesc, &Disable);
		ASSERT(Disable, "PipelineController::Create DepthStencilState fail");
	}

	void PipelineController::Rasterlizer::Init()
	{
		RasterDesc.AntialiasedLineEnable = false;
		RasterDesc.CullMode = D3D11_CULL_BACK;
		RasterDesc.DepthBias = 0;
		RasterDesc.DepthBiasClamp = 0.0f;
		RasterDesc.DepthClipEnable = false;
		RasterDesc.FillMode = D3D11_FILL_SOLID;
		RasterDesc.FrontCounterClockwise = false;
		RasterDesc.MultisampleEnable = false;
		RasterDesc.ScissorEnable = false;
		RasterDesc.SlopeScaledDepthBias = 0.0f;

		Dx11Core::Get().Device->CreateRasterizerState(&RasterDesc, &Solid);
		ASSERT(Solid, "PipelineController::Create RasterizerState fail");

		RasterDesc.FillMode = D3D11_FILL_SOLID;
		RasterDesc.DepthBias = 85;
		RasterDesc.SlopeScaledDepthBias = 5.0f;
		Dx11Core::Get().Device->CreateRasterizerState(&RasterDesc, &Shadow);
		ASSERT(Shadow, "PipelineController::Create RasterizerState fail");

		RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
		RasterDesc.CullMode = D3D11_CULL_NONE;
		Dx11Core::Get().Device->CreateRasterizerState(&RasterDesc, &Wire);
		ASSERT(Wire, "PipelineController::Create RasterizerState fail");
	}

	void PipelineController::Rasterlizer::AdjustShadowBias(int depth, float slope)
	{
		if(Shadow)
			Shadow->Release();
		RasterDesc.DepthBias += depth;
		RasterDesc.SlopeScaledDepthBias += slope;

		if (RasterDesc.DepthBias < 0.0f) RasterDesc.DepthBias = 0.0f;
		if (RasterDesc.SlopeScaledDepthBias < 0.0f) RasterDesc.SlopeScaledDepthBias = 0.0f;
		Dx11Core::Get().Device->CreateRasterizerState(&RasterDesc, &Shadow);
		
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
		ASSERT(AlphaBlend, "PipelineController::Create AlphaBlend fail");
		
		blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
		Dx11Core::Get().Device->CreateBlendState(&blendStateDesc, &None);
		ASSERT(None, "PipelineController::Create AlphaBlend fail");

		ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
		blendStateDesc.AlphaToCoverageEnable = FALSE;
		blendStateDesc.IndependentBlendEnable = FALSE;
		for (int i = 0; i < 8; ++i)
		{
			blendStateDesc.RenderTarget[i].BlendEnable = TRUE;
			blendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		Dx11Core::Get().Device->CreateBlendState(&blendStateDesc, &GBufferBlend);
		ASSERT(GBufferBlend, "PipelineController::Create GBufferBlend fail");
	}

}

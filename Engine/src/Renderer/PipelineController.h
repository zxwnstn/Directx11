#pragma once

namespace Engine {

	enum class PipelineComponent
	{
		DepthStencil,
	};

	enum class DepthStencilOpt
	{
		Enable, Disable
	};

	enum class RasterlizerOpt
	{
		Solid, Wire
	};

	enum class BlendOpt
	{
		Alpha, None
	};

	class PipelineController
	{
	private:
		void Init(const struct WindowProp& prop);
		
	public:
		PipelineController& Bind(PipelineComponent comp);
		PipelineController& Unbind(PipelineComponent comp);

		PipelineController& SetDepthStencil(DepthStencilOpt opt);
		PipelineController& SetBlend(BlendOpt opt);
		PipelineController& SetRasterize(RasterlizerOpt opt);

		inline DepthStencilOpt GetDepthStencilState() const { return m_DepthStencil.opt; }
		inline BlendOpt GetBlendOpt() const { return m_Blend.opt; }
		inline RasterlizerOpt GetRasterlizeOpt() const { return m_Rasterlizer.opt; }

		inline void Resize() { m_DepthStencil.Resize(); Bind(PipelineComponent::DepthStencil); }

	private:
		struct DepthStencil
		{
			ID3D11Texture2D* Buffer;
			ID3D11DepthStencilView* View;
			ID3D11DepthStencilState* Enable;
			ID3D11DepthStencilState* Disable;
			D3D11_TEXTURE2D_DESC DepthBufferDecs;
			D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
			DepthStencilOpt opt;
		private:
			void Resize();
			void Init(const struct WindowProp& prop);
			friend class PipelineController;
		};

		struct Rasterlizer
		{
			ID3D11RasterizerState* Solid;
			ID3D11RasterizerState* Wire;
			RasterlizerOpt opt;
		private:
			void Init();
			friend class PipelineController;
		};

		struct Blend
		{
			ID3D11BlendState* AlphaBlend;
			ID3D11BlendState* None;
			BlendOpt opt;
			float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		private:
			void Init();
			friend class PipelineController;
		};

		DepthStencil m_DepthStencil;
		Rasterlizer m_Rasterlizer;
		Blend m_Blend;

		std::set<PipelineComponent> Pipeline;

		friend class Renderer;
	};

}

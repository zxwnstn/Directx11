#pragma once

namespace Engine {

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
		Alpha, GBuffer, None
	};

	class PipelineController
	{
	private:
		void Init(const struct WindowProp& prop);
		
	public:
		PipelineController& SetDepthStencil(DepthStencilOpt opt);
		PipelineController& SetBlend(BlendOpt opt);
		PipelineController& SetRasterize(RasterlizerOpt opt);

		inline DepthStencilOpt GetDepthStencilState() const { return m_DepthStencil.opt; }
		inline BlendOpt GetBlendOpt() const { return m_Blend.opt; }
		inline RasterlizerOpt GetRasterlizeOpt() const { return m_Rasterlizer.opt; }
		
		PipelineController& SetRenderTarget(const std::string& targetTextureName);
		void ClearRTT();

	private:
		struct DepthStencil
		{
			ID3D11DepthStencilState* Enable;
			ID3D11DepthStencilState* Disable;
			ID3D11DepthStencilState* GBuffer;
			DepthStencilOpt opt;
		private:
			void Init();
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
			ID3D11BlendState* GBufferBlend;
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

		std::set<std::string> m_UsagedRTT;

		friend class Renderer;
	};

}

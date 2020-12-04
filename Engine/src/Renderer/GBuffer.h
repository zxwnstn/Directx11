#pragma once

namespace Engine {

	class GBuffer
	{
	public:
		struct DepthElem
		{
			DepthElem(uint32_t width, uint32_t height);
			void Resize(uint32_t width, uint32_t height);
;
			ID3D11Texture2D* m_Buffer;
			ID3D11ShaderResourceView* m_ResourceView;
			ID3D11DepthStencilView* m_DepthStencilView;
			ID3D11DepthStencilView* m_DepthStencilViewRead;

			D3D11_TEXTURE2D_DESC m_BufferDesc;
			D3D11_SHADER_RESOURCE_VIEW_DESC m_ResourceViewDesc;
			D3D11_DEPTH_STENCIL_VIEW_DESC m_DepthStencilViewDesc;
		};

		struct BufferElem
		{
			BufferElem(uint32_t width, uint32_t height, DXGI_FORMAT fomat);
			void Resize(uint32_t width, uint32_t height);

			ID3D11Texture2D* m_Buffer;
			ID3D11ShaderResourceView* m_ResourceView;
			ID3D11RenderTargetView* m_RenderTargetView;

			D3D11_TEXTURE2D_DESC m_BufferDesc;
			D3D11_SHADER_RESOURCE_VIEW_DESC m_ResourceViewDesc;
			D3D11_RENDER_TARGET_VIEW_DESC m_RenderTargetViewDesc;
		};
	public:
		GBuffer(uint32_t width, uint32_t height, const std::initializer_list<std::pair<std::string, DXGI_FORMAT>>& items);
		~GBuffer();

	public:
		void Bind();
		void SetRenderTarget();
		void Resize(uint32_t width, uint32_t height);

	private:
		uint32_t m_Width;
		uint32_t m_Height;
		D3D11_VIEWPORT m_ViewPortDesc;

		DepthElem* m_DepthBuffer;

		std::vector<std::string> m_BindingOrder;
		std::unordered_map<std::string, BufferElem*> m_Buffers;

		friend class Renderer;
		friend class PipelineController;
	};

}

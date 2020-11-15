#pragma once

namespace Engine {

	struct LocalDeviceSpec
	{
		uint32_t RefreshRateNum;
		uint32_t RefreshRateDenum;
		size_t VideoCardMemory;
		std::wstring Vendor;
	};

	class Dx11Core
	{
	private:
		Dx11Core() = default;

	public:
		static Dx11Core& Get();
		static void ErrorMessage(ID3D10Blob* msg);

		void Init(const struct WindowProp& prop);
		void ShutDown();
		void ClearBackBuffer();
		void Present();
		void Resize(uint32_t width, uint32_t height);
		unsigned char* GetBackBufferData();

	private:
		void GetUserDeviceInform();
		void CreateDeviceContext();
		void SetViewPort();

	private:
		LocalDeviceSpec LocalSpec;

		ID3D11Device* Device;
		ID3D11DeviceContext* Context;
		ID3D11RenderTargetView* RenderTargetView;
		ID3D11Buffer* RenderTargetBuffer;
		IDXGISwapChain* SwapChain;

		std::unique_ptr<struct WindowProp> WinProp;

		friend class PipelineController;
		friend class Renderer;
		friend class ModelBuffer;
		friend class Shader;
		friend class Texture;
	};

}

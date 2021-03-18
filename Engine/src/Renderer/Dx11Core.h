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

		uint32_t Width() const;
		uint32_t Height() const;

		void Init(const struct WindowProp& prop);
		void ShutDown();
		void Present();
		void Resize(uint32_t width, uint32_t height);

		HWND GetHWND();

	private:
		void GetUserDeviceInform();
		void CreateDeviceContext();

	public:
		ID3D11DeviceContext* Context;
		ID3D11Device* Device;

	private:
		LocalDeviceSpec LocalSpec;
		IDXGISwapChain* SwapChain;

		std::unique_ptr<struct WindowProp> WinProp;

		friend class PipelineController;
		friend class Renderer;
		friend class ModelBuffer;
		friend class Shader;
		friend class Texture;
	};

}

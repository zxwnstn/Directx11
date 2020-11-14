#pragma once

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

	void Init();
	void ShutDown();
	void ClearBackBuffer();
	void Present();

private:
	void GetUserDeviceInform();
	void CreateDeviceContext();
	void SetViewPort();

private:
	LocalDeviceSpec LocalSpec;

	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	ID3D11RenderTargetView* RenderTargetView;
	IDXGISwapChain* SwapChain;

	friend class PipelineController;
	friend class Renderer;
	friend class ModelBuffer;
	friend class Shader;
	friend class Texture;
};

	
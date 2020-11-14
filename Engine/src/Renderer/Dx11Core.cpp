#include "pch.h"

#include "Dx11Core.h"
#include "Core/Window.h"

Dx11Core& Dx11Core::Get()
{
	static Dx11Core* Inst = nullptr;
	if (Inst == nullptr)
	{
		Inst = new Dx11Core;
	}
	return *Inst;
}

void Dx11Core::Init()
{
	GetUserDeviceInform();
	CreateDeviceContext();
	SetViewPort();
}

void Dx11Core::ShutDown()
{
}

void Dx11Core::ClearBackBuffer()
{
	float color[] = { 0.1f, 0.1f, 0.1f, 0.0f };
	Context->ClearRenderTargetView(RenderTargetView, color);
}

void Dx11Core::Present()
{
	SwapChain->Present(1, 0);
}

void Dx11Core::GetUserDeviceInform()
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&factory);
	factory->EnumAdapters(0, &adapter);
	adapter->EnumOutputs(0, &adapterOutput);

	unsigned int numModes;
	adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);

	for (unsigned int i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Width == Window::Prop.Width)
		{
			if (displayModeList[i].Height == Window::Prop.Height)
			{
				LocalSpec.RefreshRateNum = displayModeList[i].RefreshRate.Numerator;
				LocalSpec.RefreshRateDenum = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	adapter->GetDesc(&adapterDesc);

	LocalSpec.VideoCardMemory = (size_t)adapterDesc.DedicatedVideoMemory / 1024 / 1024;
	LocalSpec.Vendor += adapterDesc.Description;

	std::wcout << LocalSpec.Vendor << "\n";
	std::cout << "Memory capacity : " << LocalSpec.VideoCardMemory << "MB\n";

	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();
}

void Dx11Core::CreateDeviceContext()
{
	//SwapChain Describe
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = Window::Prop.Width;
	swapChainDesc.BufferDesc.Height = Window::Prop.Height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = LocalSpec.RefreshRateNum;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = LocalSpec.RefreshRateDenum;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = Window::hWindow;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//Create Swap, Device, DeviceContext 
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
		&featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &Device, NULL
		, &Context
	);

	//Set BackBuffer
	ID3D11Texture2D* backBuffer = nullptr;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	Device->CreateRenderTargetView(backBuffer, NULL, &RenderTargetView);

	backBuffer->Release();
}

void Dx11Core::SetViewPort()
{
	D3D11_VIEWPORT viewPort;
	viewPort.Width = (float)Window::Prop.Width;
	viewPort.Height = (float)Window::Prop.Height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	Context->RSSetViewports(1, &viewPort);
}

void Dx11Core::ErrorMessage(ID3D10Blob * msg)
{
	std::cout << reinterpret_cast<const char*>(msg->GetBufferPointer()) << std::endl;
	msg->Release();
}

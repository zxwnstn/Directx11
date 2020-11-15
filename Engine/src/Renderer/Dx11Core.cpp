#include "pch.h"

#include "Dx11Core.h"
#include "Core/ModuleCore.h"

namespace Engine {

	Dx11Core& Dx11Core::Get()
	{
		static Dx11Core* Inst = nullptr;
		if (Inst == nullptr)
		{
			Inst = new Dx11Core;
		}
		return *Inst;
	}

	
	void Dx11Core::Init(const WindowProp & prop)
	{
		WinProp.reset(new WindowProp(prop));
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

	void Dx11Core::Resize(uint32_t width, uint32_t height)
	{
		Context->OMSetRenderTargets(0, 0, 0);
		RenderTargetView->Release();

		WinProp->Width = width;
		WinProp->Height = height;

		SwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		ID3D11Texture2D* backBuffer;
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

		Device->CreateRenderTargetView(backBuffer, NULL, &RenderTargetView);
		backBuffer->Release();

		SetViewPort();
	}

	unsigned char * Dx11Core::GetBackBufferData()
	{
		ID3D11Texture2D* pSurface;
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pSurface));

		Context->CopyResource(RenderTargetBuffer, pSurface);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Context->Map(RenderTargetBuffer, 0, D3D11_MAP::D3D11_MAP_READ_WRITE, 0, &mappedResource);

		return (unsigned char*)mappedResource.pData;
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
		swapChainDesc.BufferDesc.Width = WinProp->Width;
		swapChainDesc.BufferDesc.Height = WinProp->Height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = LocalSpec.RefreshRateNum;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = LocalSpec.RefreshRateDenum;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = (HWND)WinProp->hWnd;
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

		//RenderTargetTexture
		D3D11_BUFFER_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.ByteWidth = WinProp->Width * WinProp->Height * 4;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;// D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		textureDesc.MiscFlags = 0;

		// Create the texture
		Device->CreateBuffer(&textureDesc, NULL, &RenderTargetBuffer);
	}

	void Dx11Core::SetViewPort()
	{
		D3D11_VIEWPORT viewPort;
		viewPort.Width = (float)WinProp->Width;
		viewPort.Height = (float)WinProp->Height;
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

}

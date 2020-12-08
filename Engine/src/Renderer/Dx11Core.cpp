#include "pch.h"

#include "Dx11Core.h"
#include "Core/ModuleCore.h"
#include "Texture.h"

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
	}

	void Dx11Core::ShutDown()
	{
	}

	void Dx11Core::Present()
	{
		SwapChain->Present(1, 0);
	}

	void Dx11Core::Resize(uint32_t width, uint32_t height)
	{
		WinProp->Width = width;
		WinProp->Height = height;

		TextureArchive::Get("BackBuffer")->Resize(width, height);
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

		factory->MakeWindowAssociation((HWND)WinProp->hWnd, 0);
		LOG_INFO("Dx11Core::Graphic card vendor : {0}", std::string(LocalSpec.Vendor.begin(), LocalSpec.Vendor.end()));
		LOG_INFO("Dx11Core::Memory capacity : {0}MB", LocalSpec.VideoCardMemory);
		LOG_INFO("Dx11Core::DirectX Associate window : On");
		LOG_INFO("Dx11Core::DirectX vsink: On");

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
		ASSERT(SwapChain,	"Dx11Core::Create directX swapchain fail");
		ASSERT(Device,		"Dx11Core::Create directX device fail");
		ASSERT(Context,		"Dx11Core::Create directX context fail");
		
		//Create Back Buffer
		TextureArchive::CreateBackBuffer(WinProp->Width, WinProp->Height);
	}

	void Dx11Core::ErrorMessage(ID3D10Blob * msg)
	{
		LOG_ERROR("Dx11Core::On DirectX Error : {0}", reinterpret_cast<const char*>(msg->GetBufferPointer()));
		msg->Release();
	}

	uint32_t Dx11Core::Width() const { return WinProp->Width; }
	uint32_t Dx11Core::Height() const { return WinProp->Height; }
}

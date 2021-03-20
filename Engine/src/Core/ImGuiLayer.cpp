#include "ImGuiLayer.h"

#include <Renderer/Dx11Core.h>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.cpp>
#include <backends/imgui_impl_dx11.cpp>

namespace Engine {

	void ImGuiLayer::Init(int64_t windowHandle)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(HWND(windowHandle));
		ImGui_ImplDX11_Init(Dx11Core::Get().Device, Dx11Core::Get().Context);
	}

	void ImGuiLayer::ShutDown()
	{
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

}



#include "pceh.h"

#include "SandBox.h"

bool running = true;
SandBox sandBox;

Engine::vec2 prevMouse;

LRESULT __stdcall WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_MOUSEMOVE:
		sandBox.OnMouseMove(float(LOWORD(lParam)) - prevMouse.x, float(HIWORD(lParam)) - prevMouse.y);
		prevMouse.x = (float)LOWORD(lParam);
		prevMouse.y = (float)HIWORD(lParam);
		break;
	case WM_DESTROY:
		running = false;
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if(!Engine::ModuleCore::IsInitiated()) return (DefWindowProc(hWnd, iMessage, wParam, lParam));
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		Engine::Renderer::Resize(width, height);
		sandBox.OnResize();
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

int main()
{
	auto hInstance = GetModuleHandle(NULL);
	LPCTSTR appName = L"SandBox";

	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = appName;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndClass);

	auto hWindow = CreateWindow(
		L"SandBox",
		L"SandBox",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		width,
		height,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWindow, SW_SHOW);

	Engine::ModuleCore::Init({ width, height, (uint64_t)hWindow });


	sandBox.OnAttach();

	Engine::Timestep::SetTimePoint();

	while (running)
	{
		MSG msg;
		Engine::Timestep ts;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		sandBox.OnUpdate(ts);
	}
	

	return 0;
}


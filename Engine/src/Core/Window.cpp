#include "pch.h"

#include "Window.h"
#include "App.h"

HWND Window::hWindow;
HINSTANCE Window::hInstance;
WindowProp Window::Prop;

LRESULT __stdcall WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_MOUSEMOVE:
		//g_ptMouse.x = LOWORD(lParam);
		//g_ptMouse.y = HIWORD(lParam);
		break;
	case WM_DESTROY:
		App::s_Running = false;
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void Window::Init(const WindowProp& prop)
{
	Prop = prop;
	hInstance = GetModuleHandle(NULL);
	LPCTSTR appName = Prop.AppName.data();

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

	hWindow = CreateWindow(
		appName,
		appName,
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		Prop.Width,
		Prop.Height,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWindow, SW_SHOW);
}

WPARAM Window::Update()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

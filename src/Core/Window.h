#pragma once

struct WindowProp
{
	std::wstring AppName;
	int32_t Width;
	int32_t Height;
};

class Window
{
private:
	Window() = default;

public:
	static void Init(const WindowProp& prop);
	static WPARAM Update();

public:
	static HWND hWindow;
	static HINSTANCE hInstance;
	static WindowProp Prop;
};


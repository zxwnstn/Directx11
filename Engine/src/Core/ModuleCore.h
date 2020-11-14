#pragma once

struct WindowProp
{
	int32_t Width;
	int32_t Height;
	uint64_t hWnd;
};

class ModuleCore
{
private:
	ModuleCore() = default;
	~ModuleCore();

public:
	static void Init(const WindowProp& prop);
};

#pragma once

#include "Window.h"

class Layer
{
public:
	virtual void OnUpdate(float dt) = 0;
	virtual void OnAttach() = 0;
	virtual void OnDettach() = 0;
};

class App
{
public:
	App(const WindowProp& prop);
	~App();

	int32_t Run();

	void PushLayer(Layer* layer, const std::string& name);
	void PopLayer(const std::string& name);

	std::unordered_map<std::string, Layer*> Layers;
	static bool s_Running;
};

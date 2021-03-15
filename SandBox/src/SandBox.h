#pragma once

#include "scene/Scene.h"

class SandBox 
{
public:
	void Init();
	void OnUpdate(float dt);
	void OnImGui();
	/*void OnAttach();
	void OnDettach();*/
	
	void OnResize();
	void OnMouseMove(float dx, float dy);

	void controlUpdate(float dt);

private:
	std::shared_ptr<Scene> CurScene;
	std::vector<std::shared_ptr<Scene>> Scenes;
	const char* ScenesNames[10];

	bool lighting = false;
	bool gBuffer = false;
	bool shadow = false;
	bool hdr = false;
	bool gamma = false;
	bool pnTesselation = false;
	bool wire = false;

	float tFactor = 1.0f;
	float depthBias = 85.0f;
	float slopeBias = 5.0f;

	float mouseSensitive = 0.005f;
	int curSceneIdx = 0;
	bool sceneChanged = false;
	bool pathChanged = false;
	int renderingPath = 0;
	Engine::Timestep ts;
};


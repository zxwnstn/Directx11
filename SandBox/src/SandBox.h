#pragma once

#include "Scene.h"

class SandBox 
{
public:
	void Init();
	void OnUpdate(float dt);
	void OnImGui();
	
	void OnResize();
	void OnMouseMove(float dx, float dy);

	void controlUpdate(float dt);

private:
	std::shared_ptr<Scene> CurScene;
	std::vector<std::shared_ptr<Scene>> Scenes;

	bool lighting = false;
	bool gBuffer = false;
	bool shadow = false;
	bool hdr = false;
	bool gamma = false;
	bool wire = false;

	float tFactor = 1.0f;
	float depthBias = 85.0f;
	float slopeBias = 5.0f;

	float mouseSensitive = 0.005f;
	int curSceneIdx = 0;
	bool sceneChanged = false;
	bool pathChanged = false;
	int renderingPath = 0;

	bool newScene = false;
	char newSceneBuffer[100]{ 0, };

	Engine::Timestep ts;
};


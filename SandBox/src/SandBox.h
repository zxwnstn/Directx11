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

	void SaveScene(const std::string& path);
	void LoadScene(const std::string& path);

private:
	std::shared_ptr<Scene> CurScene;
	std::vector<std::shared_ptr<Scene>> Scenes;

	bool lighting = true;
	bool gBuffer = false;
	bool shadow = true;
	bool hdr = false;
	bool gamma = true;
	bool wire = false;
	bool v_Sync = true;
	bool realTimeEnv = false;

	float tFactor = 1.0f;
	int depthBias = 85;
	float slopeBias = 5.0f;

	float mouseSensitive = 0.005f;
	int curSceneIdx = 0;
	bool sceneChanged = false;
	bool pathChanged = false;
	int renderingPath = 0;
	int specMode = 0;
	int diffuseMode = 0;
	int lambertContrast = 3;

	bool newScene = false;
	bool newName = false;
	bool onSave = false;
	bool onLoad = false;
	bool onShaderLoad = false;
	char newSceneBuffer[100]{ 0, };

	wchar_t* sceneExt = L"Scene Type\0*.scene\0";
	wchar_t* shaderExt = L"Folder\0";

	Engine::Timestep ts;
	Engine::Timestep ts2;
};


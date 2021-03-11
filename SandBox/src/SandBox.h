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


	float mouseSensitive = 0.005f;
	int curSceneIdx = 0;
	bool sceneChanged = false;
};


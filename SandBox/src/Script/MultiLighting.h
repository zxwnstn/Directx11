#pragma once

#include "Script.h"

class MultiLightingScript : public Script
{
public:
	MultiLightingScript(class Scene* myScene)
		: Script(myScene, "MultiLighting")
	{}

	virtual void OnStart();
	virtual void OnStop();
	virtual void OnUpdate(float ts);
	virtual void OnMouseMove(float dx, float dy);
	virtual void OnKeyInput();

private:

	std::shared_ptr<Engine::Camera> m_ActivateCam;
	std::shared_ptr<Engine::Light> m_Green;
	std::shared_ptr<Engine::Light> m_Magenta;
	std::shared_ptr<Engine::Light> m_Blue;
	std::shared_ptr<Engine::Light> m_Yello;
	std::shared_ptr<Engine::Light> m_Red;
	std::shared_ptr<Engine::Light> m_Cyan;
	bool cyanForward = true;

	float mtR_Add = true;
	float mtG_Add = true;
	float mtB_Add = true;

};
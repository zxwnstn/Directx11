#pragma once

class Script
{
public:
	Script(class Scene* myScene, const std::string& name);

	virtual void OnStart();
	virtual void OnStop();
	virtual void OnUpdate(float ts) = 0;
	virtual void OnMouseMove(float dx, float dy) = 0;
	virtual void OnKeyInput() = 0;

	void DefaultMouseMove(float dx, float dy);
	void DefaultKeyInput();

	const std::string m_Name;
	
protected:
	class Scene* m_MyScene;
};
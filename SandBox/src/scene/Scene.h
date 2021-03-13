#pragma once

class Scene
{
public:
	virtual void OnUpdate(float dt) {};
	virtual void OnImGui() {};
	virtual void Init() {};
	virtual void ControlUpdate() {};

	const std::string GetName() const { return m_Name; }
	std::shared_ptr<class Engine::Camera> GetCurCam() { return m_Cur_cam; }
	std::vector<std::shared_ptr<struct Engine::Light>>& GetLights() { return m_Lights; }

protected:
	std::string m_Name;

	std::vector<std::shared_ptr<class Engine::Camera>> m_Cameras;
	std::vector<std::shared_ptr<struct Engine::Light>> m_Lights;

	std::shared_ptr<class Engine::Camera> m_Cur_cam;

	std::vector<std::shared_ptr<class Engine::Model2D>> m_Model2;
	std::vector<std::shared_ptr<class Engine::Model3D>> m_Model3;
};
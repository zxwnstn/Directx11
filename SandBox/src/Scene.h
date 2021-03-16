#pragma once

class Scene
{
public:
	Scene();
	Scene(const std::string& name);

	void OnUpdate(float dt);
	void OnImGui();

	void Add2DModel(std::shared_ptr<Engine::Model2D> model);
	void Add3DModel(std::shared_ptr<Engine::Model3D> model);
	void AddLight(std::shared_ptr<Engine::Light> light);
	void AddCamera(std::shared_ptr<Engine::Camera> camera);

	void SetSceneName(const std::string& name);
	const std::string& GetSceneName() const { return m_Name; }

	std::shared_ptr<class Engine::Camera> GetCurCam() { return m_Curcam; }
	std::vector<std::shared_ptr<class Engine::Camera>>& GetCams() { return m_Cameras; }
	std::vector<std::shared_ptr<struct Engine::Light>>& GetLights() { return m_Lights; }

private:
	std::string m_Name;

	std::vector<std::shared_ptr<class Engine::Camera>> m_Cameras;
	std::vector<std::shared_ptr<struct Engine::Light>> m_Lights;
	std::vector<std::shared_ptr<class Engine::Model2D>> m_Model2;
	std::vector<std::shared_ptr<class Engine::Model3D>> m_Model3;

	std::shared_ptr<class Engine::Camera> m_Curcam;

	int curModelIdx = 0;
	int newlightType = 0;
	int selectedLight = 0;
	int selectedMat = 0;
	int selectedCamera = 0;
	bool newLight = false;
	bool deleteLight = false;

	bool newModel = false;
	int selectedStatic = 0;
	int selectedSkeletal = 0;
	int newModelType = 0;
	bool deleteModel = false;
	char newModelBuffer[100]{ 0, };
	char newLightBuffer[100]{ 0, };
	float newLightPosition[3]{ 0.0f, };
	std::string selectedName;

	std::unordered_map<std::string, int> curAnimtionIdx;
	std::shared_ptr<Engine::Model3D> floor;
};
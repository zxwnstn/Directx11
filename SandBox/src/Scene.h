#pragma once

struct CameraInform
{
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & Name;
		ar & Type;
		ar & Mag;
		ar & Fov;
		ar & Near;
		ar & Far;
		ar & Translate;
		ar & Rotate;
	}

	std::string Name;
	int Type;
	float Mag;
	float Fov;
	float Near;
	float Far;
	Engine::vec3 Translate;
	Engine::vec3 Rotate;
};

struct Model2DInform
{
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & Name;
		ar & Texture;
		ar & Scale;
		ar & Translate;
		ar & Rotate;
	}
	std::string Name;
	std::string Texture;
	Engine::vec3 Scale;
	Engine::vec3 Translate;
	Engine::vec3 Rotate;
};

struct MaterialInform
{
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & Name;
		ar & Ambient;
		ar & Diffuse;
		ar & Specular;
		ar & Shiness;
		ar & Mapmode;
	}
	std::string Name;
	Engine::vec4 Ambient;
	Engine::vec4 Diffuse;
	Engine::vec4 Specular;
	float Shiness;
	int Mapmode;
};

struct AnimationInform
{
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & CurAnimation;
		ar & PlaySpeed;
		ar & Curtime;
		ar & Loop;
	}
	std::string CurAnimation = "";
	float PlaySpeed = 0.0f;
	float Curtime = 0.0f;
	bool Loop = false;
};

struct Model3DInform
{
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & Name;
		ar & MeshType;
		ar & MeshName;
		ar & Scale;
		ar & Translate;
		ar & Rotate;
		ar & Material;
		ar & AnimInfo;
	}

	std::string Name;
	int MeshType; //0 sekeltal, 1 static
	std::string MeshName;
	Engine::vec3 Scale;
	Engine::vec3 Translate;
	Engine::vec3 Rotate;
	std::vector<MaterialInform> Material;
	AnimationInform AnimInfo;
};

struct LightInform
{
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & Name;
		ar & type;
		ar & Color;
		ar & Intensity;
		ar & CasBorder1;
		ar & CasBorder2;
		ar & Range;
		ar & Position;
		ar & Direction;
		ar & InnerAngle;
		ar & OuterAngle;
	}
	std::string Name;
	int type;
	Engine::vec4 Color;
	float Intensity;
	float CasBorder1;
	float CasBorder2;
	float Range;
	Engine::vec3 Position;
	Engine::vec3 Direction;
	float InnerAngle;
	float OuterAngle;
};


struct SceneInform
{
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & SceneName;
		ar & CurrentCamIdx;
		ar & Model3d;
		ar & Light;
		ar & Camera;
	}

	std::string SceneName;
	int CurrentCamIdx;
	std::vector<Model2DInform> Model2d;
	std::vector<Model3DInform> Model3d;
	std::vector<LightInform> Light;
	std::vector<CameraInform> Camera;
};

class Scene
{
public:
	Scene();
	Scene(const SceneInform& inform);
	Scene(const std::string& name);

	void OnUpdate(float dt);
	void OnImGui();
	SceneInform Save();

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
	int curcamIdx = 0;
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
};


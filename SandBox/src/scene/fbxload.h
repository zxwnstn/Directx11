#include "Scene.h"

class FbxLoadScene : public Scene
{
public:
	void OnUpdate(float dt) override;
	void OnImGui() override;
	void Init() override;

private:
	int curModelIdx = 0;
	float animationSpeed = 0.016f;
	int newlightCnt = 0;
	int selectedLight = 0;
	int selectedMat = 0;
	bool addLight = false;
	bool deleteLight = false;
	std::vector<std::string> ModelNames;
	std::unordered_map<std::string, int> curAnimtionIdx;

	std::shared_ptr<Engine::Model3D> floor;
};